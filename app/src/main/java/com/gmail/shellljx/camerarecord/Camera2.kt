package com.gmail.shellljx.camerarecord

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.SurfaceTexture
import android.hardware.camera2.*
import android.os.*
import android.util.Log
import android.view.Surface
import androidx.annotation.RequiresApi
import androidx.core.app.ActivityCompat
import java.util.*

@RequiresApi(Build.VERSION_CODES.LOLLIPOP) class Camera2(private val context:Context) {
    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    private val mBackgroundThread = HandlerThread("Caemra2Thread")
    private var mBackgroundHandler : Handler?= null
    private val mCameraManager = context.getSystemService(Context.CAMERA_SERVICE) as CameraManager
    private var mCameraId = CameraCharacteristics.LENS_FACING_BACK.toString()
    private var mCameraSession:CameraCaptureSession? = null
    private var mPreviewBuilder: CaptureRequest.Builder? = null
    private val mCameraCount:Int = mCameraManager.cameraIdList.size
    private val mCameraIds = mCameraManager.cameraIdList
    private var mCameraDevice:CameraDevice? = null
    private var mSurfacetexture:SurfaceTexture? = null

    init {
        mBackgroundThread.start()
        mBackgroundHandler = Handler(mBackgroundThread.looper)
    }

    fun setSurfaceView(texture:SurfaceTexture){
        mSurfacetexture = texture
    }

    fun start(){
        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return
        }
        mCameraManager.openCamera(mCameraId, mCameStateCallback, mBackgroundHandler)
    }

    private val mCameStateCallback = object :CameraDevice.StateCallback() {
        override fun onOpened(p0: CameraDevice) {
            mCameraDevice = p0

            try {
                val builder = mCameraDevice!!.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW)
                mSurfacetexture?.setDefaultBufferSize(1080,1920)
                val previewSurface = Surface(mSurfacetexture!!)
                // 添加输出到屏幕的surface
                builder.addTarget(previewSurface)
                mCameraDevice!!.createCaptureSession(
                    listOf(previewSurface),
                    object : CameraCaptureSession.StateCallback() {
                        override fun onConfigured(session: CameraCaptureSession) {
                            mCameraSession = session
                            // 设置连续自动对焦和自动曝光
                            builder.set(
                                CaptureRequest.CONTROL_AF_MODE,
                                CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE
                            )
                            builder.set(
                                CaptureRequest.CONTROL_AE_MODE,
                                CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH
                            )
                            val captureRequest = builder.build()
                            try {
                                // 一直发送预览请求
                                mCameraSession!!.setRepeatingRequest(captureRequest, null, null)
                            } catch (e: CameraAccessException) {
                                e.printStackTrace()
                            }
                        }

                        override fun onConfigureFailed(session: CameraCaptureSession) {
                            Log.e("lijinxiang", "ConfigureFailed. session: mCaptureSession")
                        }
                    }, mBackgroundHandler
                ) // handle 传入 null 表示使用当前线程的 Looper
            } catch (e: CameraAccessException) {
                e.printStackTrace()
            }

        }

        override fun onDisconnected(p0: CameraDevice) {
            mCameraDevice?.close()
            mCameraDevice = null
        }

        override fun onError(p0: CameraDevice, p1: Int) {
            System.out.println("open error $p1")
        }

    }
}
