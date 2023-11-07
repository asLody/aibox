package aaiiaaii.io.aibox;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import aaiiaaii.io.aibox.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'aibox' library on application startup.
    static {
        System.loadLibrary("aibox");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            View gameView = binding.getRoot();
            gameView.requestPointerCapture();
            gameView.setFocusable(true);
            gameView.setOnCapturedPointerListener((view, motionEvent) -> {
                mouseEvent(motionEvent.getX(), motionEvent.getY(), motionEvent.getActionButton());
                return true;
            });
        }
    }

    /**
     * A native method that is implemented by the 'aibox' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void mouseEvent(float x, float y, int action);
}