package com.example.greenhouse;

import java.util.ArrayList;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;


public class PUB_DATA
{
    private static Executor executor = Executors.newSingleThreadExecutor();
    private static String s;
    public static String DATA_PUB(ArrayList val)
    {
        ArrayList<String> mkey = new ArrayList<String>();
        ArrayList mval = new ArrayList();
        mkey.add("m2m:cin");
        mval.add(val);

        executor.execute(() -> {
            HTTPHANDLER httpHandler = new HTTPHANDLER();
            String u = Link.url;
            s = httpHandler.POSTACCESS(u, mkey, mval);
        });

        return s;
    }


}
