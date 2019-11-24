package com.flyscale.chapter_4_2_2_opensl;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String TAG = "MainActivity";
    private OpenSLPlayer openSLPlayer;

    static {
        System.loadLibrary("OpenSLPlayer");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.button1).setOnClickListener(this);
        openSLPlayer = new OpenSLPlayer();
        Log.d(TAG, "Message from JNI:" + openSLPlayer.getStringFromJNI("Hello,this is Java!"));

    }

    private static final String PCM_PATH = "/mnt/sdcard/tonghuazhen_part.pcm";

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button1:
                if (openSLPlayer != null) {
                    openSLPlayer.play(PCM_PATH);
                }
                break;
        }
    }




}
