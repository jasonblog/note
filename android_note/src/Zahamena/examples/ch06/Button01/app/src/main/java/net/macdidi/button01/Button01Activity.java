package net.macdidi.button01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

public class Button01Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_button01);
    }

    public void sayHello(View view)
    {
        Toast.makeText(
            this, "Hello! Button!", Toast.LENGTH_SHORT).show();
    }
}
