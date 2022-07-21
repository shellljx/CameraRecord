package com.gmail.shellljx.camerarecord

import android.content.Context
import android.graphics.SurfaceTexture
import android.os.Build
import android.view.Surface
import androidx.annotation.RequiresApi

@RequiresApi(Build.VERSION_CODES.LOLLIPOP)
class Recorder(context: Context) :SurfaceTexture.OnFrameAvailableListener{

    private val camera = Camera2(context)
    private var surfaceTexture:SurfaceTexture? = null
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun surfaceCreateJNI(surface: Surface)

    external fun onFrameAvailable()

    fun startPreviewFromNative(texture: Int) {
        System.out.println("$texture")
        surfaceTexture = SurfaceTexture(texture)
        surfaceTexture!!.setOnFrameAvailableListener(this)
        camera.setSurfaceView(surfaceTexture!!)
        camera.start()
    }

    fun updateTexImageFromNative(){
        surfaceTexture?.updateTexImage()
    }

    private fun getTextureMatrix():FloatArray{
        val matrix = FloatArray(16)
        surfaceTexture?.getTransformMatrix(matrix)
        return matrix
    }

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }

    override fun onFrameAvailable(p0: SurfaceTexture?) {
        System.out.println("frame available")
        onFrameAvailable()
    }
}
