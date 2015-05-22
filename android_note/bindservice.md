# service组件-bindService源码分析


因为有前面分析startService的代码实现过程，则对于bindService的代码分析就不用那么详细介绍，在介绍流程的同时更关注一些细节上的部分。



首先，bindService也是通过 ContextWrapper.bindService，再到ContextImpl的bindService，然后是bindServiceCommon，需要注意的是，传入的ServiceConnection被转换成IServiceConnection类型，

```
private boolean bindServiceCommon(Intent service, ServiceConnection conn,
                                  int flags,
                                  UserHandle user)
{
    IServiceConnection sd;
    sd = mPackageInfo.getServiceDispatcher(conn, getOuterContext(),
                                           mMainThread.getHandler(), flags);

    int res = ActivityManagerNative.getDefault().bindService(
                  mMainThread.getApplicationThread(), getActivityToken(),
                  service, service.resolveTypeIfNeeded(getContentResolver()),
                  sd, flags, user.getIdentifier());
}

```

接下去是进入AMS的bindService，再调用ActiveServices.java 的bindServiceLocked，它会把IServiceConnection实例存放到ConnectionRecord里面，并执行bringUpServiceLocked，


```
int bindServiceLocked(IApplicationThread caller, IBinder token,
                      Intent service, String resolvedType,
                      IServiceConnection connection, int flags, int userId)
{

    ConnectionRecord c = new ConnectionRecord(b, activity,
            connection, flags, clientLabel, clientIntent);

    IBinder binder = connection.asBinder();

    if ((flags & Context.BIND_AUTO_CREATE) != 0) {
        s.lastActivity = SystemClock.uptimeMillis();

        if (bringUpServiceLocked(s, service.getFlags(), callerFg, false) != null) {
            return 0;
        }
    }

    if (s.app != null && b.intent.received) {
        // Service is already running, so we can immediately
        // publish the connection.
        try {
            c.conn.connected(s.name, b.intent.binder);
        } catch (Exception e) {
            Slog.w(TAG, "Failure sending service " + s.shortName
                   + " to connection " + c.conn.asBinder()
                   + " (in " + c.binding.client.processName + ")", e);
        }

        // If this is the first app connected back to this binding,
        // and the service had previously asked to be told when
        // rebound, then do so.

        if (b.intent.apps.size() == 1 && b.intent.doRebind) {
            requestServiceBindingLocked(s, b.intent, callerFg, true);
        }

    } else if (!b.intent.requested) {
        requestServiceBindingLocked(s, b.intent, callerFg, false);
    }
}
```

根据之前的分析ServiceLocked会调用realStartServiceLocked，而realStartServiceLocked则先调用scheduleCreateService，完成service的创建和Oncreate（）的执行，然后执行requestServiceBindingsLocked，这个是bind服务相关处理，最后是sendServiceArgsLocked，这个是Start服务的处理。


```
private final void realStartServiceLocked(ServiceRecord r,
        ProcessRecord app, boolean execInFg) throws RemoteException {
    app.thread.scheduleCreateService(r, r.serviceInfo, mAm.compatibilityInfoForPackageLocked(r.serviceInfo.applicationInfo), app.repProcState);
    requestServiceBindingsLocked(r, execInFg);
    sendServiceArgsLocked(r, execInFg, true);
}
```

requestServiceBindingsLocked再调用ActivityThread的方法scheduleBindService，在ActivityThread.java 中，它发出一个BIND_SERVICE事件，被handleBindService处理，

```
void publishServiceLocked(ServiceRecord r, Intent intent, IBinder service)
{
    for (int conni = r.connections.size() - 1; conni >= 0; conni--) {
        ArrayList<ConnectionRecord> clist = r.connections.valueAt(conni);
        for (int i = 0; i < clist.size(); i++) {
            ConnectionRecord c = clist.get(i);
            try {
                c.conn.connected(r.name, service);
            }
            serviceDoneExecutingLocked(r, mDestroyingServices.contains(r), false);
        }
    }
}
```

这里主要调用到c.conn.connected，c就是ConnectionRecord，其成员conn是一个IServiceConnection类型实例，这在前面有提到，connected则是其实现类的方法。

对于IServiceConnection，它是一个接口，位置在(frameworks\base): core/java/android/app/IServiceConnection.aidl，aidl定义如下，它只有一个接口方法connected，
```
oneway interface IServiceConnection {
    void connected(in ComponentName name, IBinder service);
}
```

其服务端的实现在LoadedApk.java，如下，InnerConnection类是在ServiceDispatcher的内部类，并在ServiceDispatcher的构造函数里面实例化的，其方法connected也是调用的ServiceDispatcher的方法connected，

```
private static class InnerConnection extendsIServiceConnection.Stub
{
    final WeakReference<LoadedApk.ServiceDispatcher> mDispatcher;
    InnerConnection(LoadedApk.ServiceDispatcher sd)
    {
        mDispatcher = new WeakReference<LoadedApk.ServiceDispatcher>(sd);
    }

    public void connected(ComponentName name,
                          IBinder service) throws RemoteException {
        LoadedApk.ServiceDispatcher sd = mDispatcher.get();
        if (sd != null)
        {
            sd.connected(name, service);
        }
    }
}

ServiceDispatcher(ServiceConnection conn,
                  Context context, Handler activityThread, int flags)
{
    mIServiceConnection = new InnerConnection(this);
    mConnection = conn;
    mContext = context;

    mActivityThread = activityThread;
    mLocation = new ServiceConnectionLeaked(null);
    mLocation.fillInStackTrace();
    mFlags = flags;
}
```

这里就再回到我们前面的ContextImpl里面bindServiceCommon方法里面，这里进行ServiceConnection转化为IServiceConnection时，调用了mPackageInfo.getServiceDispatcher，mPackageInfo就是一个LoadedApk实例，

```
/*package*/ LoadedApk mPackageInfo;

private boolean bindServiceCommon(Intent service, ServiceConnection conn,
                                  int flags,
                                  UserHandle user)
{
    IServiceConnection sd;
    sd = mPackageInfo.getServiceDispatcher(conn, getOuterContext(),
                                           mMainThread.getHandler(), flags);
}
```

所以，getServiceDispatcher会创建一个ServiceDispatcher实例，并将ServiceDispatcher实例和ServiceConnection实例形成KV对，并在ServiceDispatcher的构造函数里将ServiceConnection实例c赋值给ServiceConnection的成员变量mConnection，

```
public final IServiceConnection getServiceDispatcher(ServiceConnection c,
        Context context, Handler handler, int flags)
{
    synchronized(mServices) {
        LoadedApk.ServiceDispatcher sd = null;
        ArrayMap<ServiceConnection, LoadedApk.ServiceDispatcher> map = mServices.get(
                    context);

        if (map != null) {
            sd = map.get(c);
        }

        if (sd == null) {
            sd = new ServiceDispatcher(c, context, handler, flags);

            if (map == null) {
                map = new ArrayMap<ServiceConnection, LoadedApk.ServiceDispatcher>();
                mServices.put(context, map);
            }

            map.put(c, sd);
        }
    }
}
```

这样，在执行ServiceDispatcher的connected方法时，就会调用到ServiceConnection的
onServiceConnected，完成绑定ServiceConnection的触发。

```
public void doConnected(ComponentName name, IBinder service)
{
    if (old != null) {
        mConnection.onServiceDisconnected(name);
    }

    // If there is a new service, it is now connected.
    if (service != null) {
        mConnection.onServiceConnected(name, service);
    }
}
```

至此，就执行完了bindService的主要过程。
