package net.macdidi.audio02;

import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.SeekBar;

public class Audio02Activity extends AppCompatActivity
{

    // 控制左右聲道用的SeekBar元件
    private SeekBar volume_left, volume_right;
    // 左右聲道音量，0.0F ~ 1.0F
    private float leftVolume = 1.0F, rightVolume = 1.0F;

    private SoundPool soundPool;
    private int soundId02, soundId03;
    private int playing;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio02);

        processViews();
        processControllers();

        // 建立SoundPool物件
        // 第一個參數設定音效最大數量
        // 第二個參數設定音效種類，通常指定為AudioManager.STREAM_MUSIC
        // 第三個參數設定播放音質，目前沒有作用
        soundPool = new SoundPool(3, AudioManager.STREAM_MUSIC, 0);
        // 載入指定資源編號的音樂並取得編號，第三個參數目前沒有作用
        soundId02 = soundPool.load(this, R.raw.sound02, 0);
        soundId03 = soundPool.load(this, R.raw.sound03, 0);
    }

    private void processViews()
    {
        volume_left = (SeekBar) findViewById(R.id.volume_left);
        volume_right = (SeekBar) findViewById(R.id.volume_right);
    }

    private void processControllers()
    {
        // 建立控制左右聲道的進度改變監聽物件
        SeekBar.OnSeekBarChangeListener listener =
        new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar,
                                          int progress,
                                          boolean fromUser) {
                int id = seekBar.getId();

                // 控制左聲道音量
                if (id == R.id.volume_left) {
                    leftVolume = progress / 10.0F;
                }
                // 控制右聲道音量
                else if (id == R.id.volume_right) {
                    rightVolume = progress / 10.0F;
                }

                // 設定指定編號的左右聲道音量
                soundPool.setVolume(playing, leftVolume, rightVolume);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        };

        volume_left.setOnSeekBarChangeListener(listener);
        volume_right.setOnSeekBarChangeListener(listener);
    }

    public void clickPlay02(View view)
    {
        // 播放第一個音效
        soundPool.play(soundId02, leftVolume, rightVolume, 0, 0, 1.0F);
        playing = soundId02;
    }

    public void clickPlay03(View view)
    {
        // 播放第二個音效
        soundPool.play(soundId03, leftVolume, rightVolume, 0, 0, 1.0F);
        playing = soundId03;
    }

}
