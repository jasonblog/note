package com.archer.rainbow.client;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

public class BindServiceDemoClient extends Activity
{
    // 将该变量赋值给Message的replyTo字段，然后传送至Service,这样的话，
    // Service 可以给该Activity以响应
    private Messenger messenger;

    private static final int MSG_REGISTER_CLIENT = 1;
    private static final int MSG_UNREGISTER_CLIENT = 2;
    private static final int MSG_SET_VALUE = 3;

    private Button bindSer;
    private Button unbindSer;
    private Button clearLog;
    private TextView log;
    private Intent intent;
    private ScrollView logView;

    protected boolean isBound;

    private class IncomingHandler extends Handler
    {
        /*
         * 处理从Service发送至该Activity的消息
         * (non-Javadoc)
         * @see android.os.Handler#handleMessage(android.os.Message)
         */
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what) {
            case MSG_SET_VALUE:
                Toast.makeText(BindServiceDemoClient.this,
                               "set value as: " + msg.arg1, Toast.LENGTH_SHORT)
                .show();
                break;

            default:
                super.handleMessage(msg);
            }
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        bindSer = (Button) findViewById(R.id.bindSer);
        unbindSer = (Button) findViewById(R.id.unbindSer);

        clearLog = (Button) findViewById(R.id.clearLog);

        log = (TextView) findViewById(R.id.log);

        logView = (ScrollView) findViewById(R.id.logView);

        bindSer.setOnClickListener(btnListener);
        unbindSer.setOnClickListener(btnListener);

        clearLog.setOnClickListener(btnListener);

        // 通过该Intent将启动 BindServiceDemo_Service工程中的相应Service
        intent = new Intent();
        intent.setAction(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_DEFAULT);

        messenger = new Messenger(new IncomingHandler());
    }

    private OnClickListener btnListener = new OnClickListener()
    {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
            case R.id.bindSer:
                updateLog("Bind Service pressed");
                doBindService();
                break;

            case R.id.unbindSer:
                updateLog("Unbind Service pressed");
                doUnbindService();
                break;

            case R.id.clearLog:
                clearLog();
                break;

            default:
                break;
            }
        }

    };

    private void doUnbindService()
    {
        if (isBound) {
            unbindService(myRemoteServiceConnection);
            isBound = false;
        }
    }

    private void doBindService()
    {
        Log.i("bind", "begin to bind");
        isBound = bindService(intent, myRemoteServiceConnection,
                              Context.BIND_AUTO_CREATE);
        updateLog("service bound: " + (isBound ? "Successfully" : "failed"));
    }

    private ServiceConnection myRemoteServiceConnection = new ServiceConnection()
    {
        public void onServiceConnected(android.content.ComponentName name,
                                       android.os.IBinder service) {
            updateLog("myServiceConnection.onServiceConnected");
            // 客户端 与 服务 不在同一个进程中的话，所以不可以进行显示强制类型转换的，
            // 因为，通过Debug，可以发现此时传进来的 Service 的类型是 BinderProxy
            isBound = true;
            // 使用从Service返回的IBinder来生成一个Messenger
            Messenger serviceMessenger = new Messenger(service);
            // 生成一个Message
            Message msg = Message.obtain();
            msg.what = MSG_REGISTER_CLIENT;
            msg.replyTo = messenger;

            try {
                // 向Service 发送Message
                serviceMessenger.send(msg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }

            msg = Message.obtain();
            msg.what = MSG_SET_VALUE;
            msg.replyTo = messenger;
            msg.arg1 = 10;

            try {
                serviceMessenger.send(msg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        };

        public void onServiceDisconnected(android.content.ComponentName name) {
            updateLog("myServiceConnection.onServiceDisconnected");
            isBound = false;
        };
    };

    private void updateLog(final String text)
    {
        CharSequence ch = log.getText();
        log.setText((ch == null || ch.length() == 0) ? text : ch.toString()
                    + "\r\n" + text);
        logView.fullScroll(ScrollView.FOCUS_DOWN);
    }

    private void clearLog()
    {
        log.setText("");
        logView.fullScroll(ScrollView.FOCUS_UP);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        doUnbindService();
    }
}