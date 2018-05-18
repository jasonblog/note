package com.demo.asus.fileobserver;

import android.os.FileObserver;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;

import java.io.FileReader;


import java.util.ArrayList;

import java.util.Stack;


public class RecursiveFileObserver
{

    private ArrayList<SingleFileObserver> mSingleObservers = new
    ArrayList<SingleFileObserver>();


    public RecursiveFileObserver(String path)
    {

        //解析子目录
        Stack<String> pathStack = new Stack<String>();
        pathStack.push(path);

        while (!pathStack.isEmpty()) {
            String parentPath = pathStack.pop();

            if (mSingleObservers.add(new SingleFileObserver(parentPath))) {
                Log.d("C&C", "add observer success" + parentPath);
            }

            File parent = new File(parentPath);

            if (parent.isDirectory()) {
                File[] files = parent.listFiles();

                for (int i = 0; i < files.length; i++) {
                    File f = files[i];

                    if (f.isDirectory() &&
                        (f.getName().equals(".") || f.getName().equals(".."))) {
                        //跳过 "." ".." 目录
                    } else {
                        pathStack.push(f.toString());
                        //pathStack.push(f.getAbsolutePath());
                        Log.d("C&C", "file list:" + f.toString());
                    }
                }
            }

        }

    }


    public void startWatching()
    {
        for (int i = 0; i < mSingleObservers.size(); i++) {
            mSingleObservers.get(i).startWatching();
        }

    }



    public void stopWatching()
    {
        for (int i = 0; i < mSingleObservers.size(); i++) {
            mSingleObservers.get(i).stopWatching();
        }
    }



    private class SingleFileObserver extends FileObserver
    {

        protected String mPath ;
        protected int mMask;
        public static final int DEFAULT_MASK = CREATE | MODIFY | DELETE;


        public SingleFileObserver(String path)
        {
            this(path, DEFAULT_MASK);
        }

        public SingleFileObserver(String path, int mask)
        {
            super(path, mask);
            mPath = path;
            mMask = mask;
        }

        @Override
        public void onEvent(int i, String s)
        {
            int event = i & FileObserver.ALL_EVENTS;

            switch (event) {
            case MODIFY:

                //查看是否被调试
                if (isDebugged(s)) {
                    Log.d("C&C", "is debugged");
                }
            }
        }
    }


    public static boolean isDebugged(String path)
    {

        BufferedReader reader = null;

        try {
            FileReader fileReader = new FileReader(path);
            reader = new BufferedReader(fileReader);
            String str;

            while ((str = reader.readLine()) != null) {
                if (str.contains("TracerPid")) {
                    int tracerPid = Integer.parseInt(str.substring(11));

                    if (tracerPid != 0) {
                        return true;
                    } else {
                        return false;
                    }
                }
            }


        } catch (Exception exception) {

        }

        return true;
    }
}
