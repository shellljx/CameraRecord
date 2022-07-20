package com.gmail.shellljx.camerarecord

import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Surface
import android.view.SurfaceHolder
import android.widget.TextView
import androidx.annotation.RequiresApi
import com.gmail.shellljx.camerarecord.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var recorder :Recorder

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        recorder = Recorder(this)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.surfaceview.holder.addCallback(object :SurfaceHolder.Callback{
            override fun surfaceCreated(p0: SurfaceHolder) {
                System.out.println("surface create")
                recorder.surfaceCreateJNI(p0.surface)
            }

            override fun surfaceChanged(p0: SurfaceHolder, p1: Int, p2: Int, p3: Int) {

            }

            override fun surfaceDestroyed(p0: SurfaceHolder) {

            }

        })
    }
}
