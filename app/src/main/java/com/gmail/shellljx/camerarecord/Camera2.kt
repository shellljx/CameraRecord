package com.gmail.shellljx.camerarecord

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.SurfaceTexture
import android.hardware.camera2.*
import android.os.Build
import android.view.Surface
import androidx.annotation.RequiresApi
import androidx.core.app.ActivityCompat

@RequiresApi(Build.VERSION_CODES.LOLLIPOP) class Camera2(private val context:Context) {
    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    private val mCameraManager = context.getSystemService(Context.CAMERA_SERVICE) as CameraManager
    private var mCameraId = CameraCharacteristics.LENS_FACING_BACK.toString()
    private var mCameraSession:CameraCaptureSession? = null
    private var mPreviewBuilder: CaptureRequest.Builder? = null
    private val mCameraCount:Int = mCameraManager.cameraIdList.size
    private val mCameraIds = mCameraManager.cameraIdList
    private var mCameraDevice:CameraDevice? = null
    private var mSurfacetexture:SurfaceTexture? = null

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
        mCameraManager.openCamera(mCameraId, mCameStateCallback, null)
    }

    private val mCameStateCallback = object :CameraDevice.StateCallback() {
        override fun onOpened(p0: CameraDevice) {
            mCameraDevice = p0

            mSurfacetexture?.let {
                mPreviewBuilder = mCameraDevice?.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW)
                val surface = Surface(it)
                it.setDefaultBufferSize(1080,1920)
                mPreviewBuilder?.addTarget(surface)
                mCameraDevice?.createCaptureSession(arrayListOf(surface), object : CameraCaptureSession.StateCallback() {
                    override fun onConfigured(p0: CameraCaptureSession) {
                        if (mCameraDevice == null){
                            return
                        }
                        mCameraSession = p0
                        val previewRequest = mPreviewBuilder?.build()?:return
                        mCameraSession?.setRepeatingRequest(previewRequest, null, null)
                    }

                    override fun onConfigureFailed(p0: CameraCaptureSession) {
                        System.out.println("config error")
                    }

                }, null)
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
