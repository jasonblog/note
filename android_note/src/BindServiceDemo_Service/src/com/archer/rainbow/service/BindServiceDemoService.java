package com.archer.rainbow.service;

import java.util.ArrayList;
import java.util.List;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;

public class BindServiceDemoService extends Service
{
    private Messenger messenger;
    private List<Messenger> allClients;

    private static final int MSG_REGISTER_CLIENT = 1;
    private static final int MSG_UNREGISTER_CLIENT = 2;
    private static final int MSG_SET_VALUE = 3;

    private class IncomingHandler extends Handler
    {

        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what) {
            case MSG_REGISTER_CLIENT:
                allClients.add(msg.replyTo);
                break;

            case MSG_UNREGISTER_CLIENT:
                allClients.remove(msg.replyTo);
                break;

            case MSG_SET_VALUE:
                int value = msg.arg1;

                for (int i = 0; i < allClients.size(); i++) {
                    try {
                        allClients.get(i).send(
                            Message.obtain(null, MSG_SET_VALUE, value,
                                           0));
                    } catch (RemoteException e) {
                        allClients.remove(i);
                    }
                }

                break;

            default:
                super.handleMessage(msg);
            }
        }

    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return messenger.getBinder();
    }

    @Override
    public void onCreate()
    {
        allClients = new ArrayList<Messenger>();
        messenger = new Messenger(new IncomingHandler());
    }

    @Override
    public void onDestroy()
    {
        allClients.clear();
        allClients = null;
    }
}