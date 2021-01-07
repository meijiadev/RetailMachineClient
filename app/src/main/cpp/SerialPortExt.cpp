/*
 * Copyright (C) 2017 10 31 Fengmeiyin
 */

#include "jni.h"
#include <stdlib.h>

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <jni.h>

#include "android/log.h"
static const char *TAG="SerialPortJNI";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

static jfieldID field_context;

static speed_t getBaudrate(jint baudrate)
{
	switch(baudrate) {
	case 0: return B0;
	case 50: return B50;
	case 75: return B75;
	case 110: return B110;
	case 134: return B134;
	case 150: return B150;
	case 200: return B200;
	case 300: return B300;
	case 600: return B600;
	case 1200: return B1200;
	case 1800: return B1800;
	case 2400: return B2400;
	case 4800: return B4800;
	case 9600: return B9600;
	case 19200: return B19200;
	case 38400: return B38400;
	case 57600: return B57600;
	case 115200: return B115200;
	case 230400: return B230400;
	case 460800: return B460800;
	case 500000: return B500000;
	case 576000: return B576000;
	case 921600: return B921600;
	case 1000000: return B1000000;
	case 1152000: return B1152000;
	case 1500000: return B1500000;
	case 2000000: return B2000000;
	case 2500000: return B2500000;
	case 3000000: return B3000000;
	case 3500000: return B3500000;
	case 4000000: return B4000000;
	default: return -1;
	}
}

#define ONE_OPEN 1

static void jniSetFileDescriptorOfFD(JNIEnv* env, jobject fileDescriptor, int value) {
	jclass cFileDescriptor = env->FindClass("java/io/FileDescriptor");	
    static jfieldID fid = env->GetFieldID(cFileDescriptor, "descriptor", "I");
    env->SetIntField(fileDescriptor, fid, value);
}

static jobject jniCreateFileDescriptor(JNIEnv* env, int fd) {
    //JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
	jclass cFileDescriptor =env->FindClass("java/io/FileDescriptor");
    static jmethodID ctor = env->GetMethodID(cFileDescriptor, "<init>", "()V");
    jobject fileDescriptor = env->NewObject(cFileDescriptor, ctor);
    // NOTE: NewObject ensures that an OutOfMemoryError will be seen by the Java
    // caller if the alloc fails, so we just return NULL when that happens.
    if (fileDescriptor != NULL)  {
        jniSetFileDescriptorOfFD(env, fileDescriptor, fd);
    }
    return fileDescriptor;
}

#ifndef ONE_OPEN

static int jniGetFDFromFileDescriptor(JNIEnv* env, jobject fileDescriptor) {
    JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
	jclass cFileDescriptor = e->FindClass("java/io/FileDescriptor");
	
    static jfieldID fid = e->GetFieldID(cFileDescriptor, "descriptor", "I");
    if (fileDescriptor != NULL) {
        return e->GetIntField(fileDescriptor, fid);
    } else {
        return -1;
    }
}

static void
android_hardware_SerialPort_open(JNIEnv *env, jobject thiz, jobject fileDescriptor, jint baudrate)
{
	speed_t speed;

	/* Check arguments */
	{
		speed = getBaudrate(baudrate);
		if (speed == -1) {
			/* TODO: throw an exception */
			LOGE("Invalid baudrate");
			return ;
		}
	}

    int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    // duplicate the file descriptor, since ParcelFileDescriptor will eventually close its copy
    fd = dup(fd);
    if (fd < 0) {
        //jniThrowException(env, "java/io/IOException", "Could not open serial port");
        return;
    }
    env->SetIntField(thiz, field_context, fd);

    struct termios tio;
    if (tcgetattr(fd, &tio))
        memset(&tio, 0, sizeof(tio));

    tio.c_cflag =  speed | CS8 | CLOCAL | CREAD;
    // Disable output processing, including messing with end-of-line characters.
    tio.c_oflag &= ~OPOST;
    tio.c_iflag = IGNPAR;
    tio.c_lflag = 0; /* turn of CANON, ECHO*, etc */
    /* no timeout but request at least one character per read */
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN] = 1;
    tcsetattr(fd, TCSANOW, &tio);
    tcflush(fd, TCIFLUSH);
}

#else
static jobject
android_hardware_SerialPort_open(JNIEnv *env,  jclass thiz, jstring path, jint baudrate/*, jint flags*/)
{
	int fd;
	int flags = 0;
	speed_t speed;
	jobject mFileDescriptor;

	/* Check arguments */
	{
		speed = getBaudrate(baudrate);
		if (speed == -1) {
			/* TODO: throw an exception */
			LOGE("Invalid baudrate");
			return NULL;
		}
	}

	/* Opening device */
	{
		jboolean iscopy;
		const char *path_utf = env->GetStringUTFChars(path, &iscopy);
		LOGD("Opening serial port %s with flags 0x%x", path_utf, O_RDWR | flags);
		fd = open(path_utf, O_RDWR | flags);
		LOGD("open() fd = %d", fd);
		env->ReleaseStringUTFChars( path, path_utf);
		if (fd == -1)
		{
			/* Throw an exception */
			LOGE("Cannot open port");
			/* TODO: throw an exception */
			return NULL;
		}
	}
	
    // duplicate the file descriptor, since ParcelFileDescriptor will eventually close its copy
    fd = dup(fd);
    if (fd < 0) {
        //jniThrowException(env, "java/io/IOException", "Could not open serial port");
        return NULL;
    }
	env->SetIntField(thiz, field_context, fd);
	
	/* Configure device */
	{
		struct termios tio;
		if (tcgetattr(fd, &tio))
			memset(&tio, 0, sizeof(tio));

		tio.c_cflag =  speed | CS8 | CLOCAL | CREAD;
		// Disable output processing, including messing with end-of-line characters.
		tio.c_oflag &= ~OPOST;
		tio.c_iflag = IGNPAR;
		tio.c_lflag = 0; /* turn of CANON, ECHO*, etc */
		/* no timeout but request at least one character per read */
		tio.c_cc[VTIME] = 0;
		tio.c_cc[VMIN] = 1;
		tcsetattr(fd, TCSANOW, &tio);
		tcflush(fd, TCIFLUSH);
	}

	/* Create a corresponding file descriptor */
	/*
	{
		jclass cFileDescriptor = env->FindClass("java/io/FileDescriptor");
		jmethodID iFileDescriptor = env->GetMethodID(cFileDescriptor, "<init>", "()V");
		jfieldID descriptorID = env->GetFieldID(cFileDescriptor, "descriptor", "I");
		mFileDescriptor = env->NewObject(cFileDescriptor, iFileDescriptor);
		env->SetIntField(mFileDescriptor, descriptorID, (jint)fd);
	}

	return mFileDescriptor;
	*/
	
    jclass clazz = env->FindClass("android/os/ParcelFileDescriptor");
	
    jobject fileDescriptor = jniCreateFileDescriptor(env, fd);
    if (fileDescriptor == NULL) {
        return NULL;
    }
    return env->NewObject(
    		(jclass) env->NewGlobalRef(clazz),
    		 env->GetMethodID(clazz, "<init>", "(Ljava/io/FileDescriptor;)V"),
			 fileDescriptor);	
}
#endif

static void
android_hardware_SerialPort_close(JNIEnv *env, jobject thiz)
{
    int fd = env->GetIntField(thiz, field_context);
    close(fd);
    env->SetIntField(thiz, field_context, -1);
}

static jint
android_hardware_SerialPort_read_array(JNIEnv *env, jobject thiz, jbyteArray buffer, jint length)
{
    int fd = env->GetIntField(thiz, field_context);
    jbyte* buf = (jbyte *)malloc(length);
    if (!buf) {
        //jniThrowException(env, "java/lang/OutOfMemoryError", NULL);
        return -1;
    }

    int ret = read(fd, buf, length);
    if (ret > 0) {
        // copy data from native buffer to Java buffer
        env->SetByteArrayRegion(buffer, 0, ret, buf);
    }

    free(buf);
    if (ret < 0){
        //jniThrowException(env, "java/io/IOException", NULL);
	}
    return ret;
}

static jint
android_hardware_SerialPort_read_direct(JNIEnv *env, jobject thiz, jobject buffer, jint length)
{
    int fd = env->GetIntField(thiz, field_context);

    jbyte* buf = (jbyte *)env->GetDirectBufferAddress(buffer);
    if (!buf) {
        //jniThrowException(env, "java/lang/IllegalArgumentException", "ByteBuffer not direct");
        return -1;
    }

    int ret = read(fd, buf, length);
    if (ret < 0){
        //jniThrowException(env, "java/io/IOException", NULL);
	}
    return ret;
}

static void
android_hardware_SerialPort_write_array(JNIEnv *env, jobject thiz, jbyteArray buffer, jint length)
{
    int fd = env->GetIntField(thiz, field_context);
    jbyte* buf = (jbyte *)malloc(length);
    if (!buf) {
        //jniThrowException(env, "java/lang/OutOfMemoryError", NULL);
        return;
    }
    env->GetByteArrayRegion(buffer, 0, length, buf);

    jint ret = write(fd, buf, length);
    free(buf);
    if (ret < 0){
        //jniThrowException(env, "java/io/IOException", NULL);
	}
}

static void
android_hardware_SerialPort_write_direct(JNIEnv *env, jobject thiz, jobject buffer, jint length)
{
    int fd = env->GetIntField(thiz, field_context);

    jbyte* buf = (jbyte *)env->GetDirectBufferAddress(buffer);
    if (!buf) {
        //jniThrowException(env, "java/lang/IllegalArgumentException", "ByteBuffer not direct");
        return;
    }
    int ret = write(fd, buf, length);
    if (ret < 0){
        //jniThrowException(env, "java/io/IOException", NULL);
	}
}

static void
android_hardware_SerialPort_send_break(JNIEnv *env, jobject thiz)
{
    int fd = env->GetIntField(thiz, field_context);
    tcsendbreak(fd, 0);
}

#ifndef ONE_OPEN
static jobject android_server_SerialService_open(JNIEnv *env, jobject thiz, jstring path)
{
    const char *pathStr = env->GetStringUTFChars(path, NULL);

    int fd = open(pathStr, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        LOGE("could not open %s", pathStr);
        env->ReleaseStringUTFChars(path, pathStr);
        return NULL;
    }
    env->ReleaseStringUTFChars(path, pathStr);
	

    jclass clazz = env->FindClass("android/os/ParcelFileDescriptor");
	
    jobject fileDescriptor = jniCreateFileDescriptor(env, fd);
    if (fileDescriptor == NULL) {
        return NULL;
    }
    return env->NewObject(
    		(jclass) env->NewGlobalRef(clazz),
    		 env->GetMethodID(clazz, "<init>", "(Ljava/io/FileDescriptor;)V"),
			 fileDescriptor);

			 
	/* Create a corresponding file descriptor */
/*	
	jobject mFileDescriptor;
	//{
		jclass cFileDescriptor = env->FindClass("java/io/FileDescriptor");
		jmethodID iFileDescriptor = env->GetMethodID(cFileDescriptor, "<init>", "()V");
		jfieldID descriptorID = env->GetFieldID(cFileDescriptor, "descriptor", "I");
		mFileDescriptor = env->NewObject(cFileDescriptor, iFileDescriptor);
		env->SetIntField(mFileDescriptor, descriptorID, (jint)fd);
	//}	
	return mFileDescriptor;
*/	
}
#endif

static JNINativeMethod method_table[] = {
#ifndef ONE_OPEN	
    {"native_open",             "(Ljava/io/FileDescriptor;I)V",
                                        (void *)android_hardware_SerialPort_open},
    //{"native_open",             "(Ljava/lang/String;I)V",
    //                                    (void *)android_hardware_SerialPort_open},										
										
	{"native_open_file",       "(Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;",
									(void*)android_server_SerialService_open },
	//{"native_open_file",       "(Ljava/lang/String;)Ljava/io/FileDescriptor;",
	//								(void*)android_server_SerialService_open },	
#else
    {"native_open",             "(Ljava/lang/String;I)Landroid/os/ParcelFileDescriptor;",
                                        (void *)android_hardware_SerialPort_open},
#endif	
    {"native_close",            "()V",  (void *)android_hardware_SerialPort_close},
    {"native_read_array",       "([BI)I",
                                        (void *)android_hardware_SerialPort_read_array},
    {"native_read_direct",      "(Ljava/nio/ByteBuffer;I)I",
                                        (void *)android_hardware_SerialPort_read_direct},
    {"native_write_array",      "([BI)V",
                                        (void *)android_hardware_SerialPort_write_array},
    {"native_write_direct",     "(Ljava/nio/ByteBuffer;I)V",
                                        (void *)android_hardware_SerialPort_write_direct},
    {"native_send_break",       "()V",  (void *)android_hardware_SerialPort_send_break},
};

#if 1
static const char *classPathName = "com/hardware/SerialPort/SerialPort";  // ????????,????????????java???????????

/*
* Register several native methods for one class.
*/
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
	
	//LOGE("set field_context");
	
	field_context = env->GetFieldID(clazz, "mNativeContext", "I");
	LOGD("SerialPort.mNativeContext:%d",field_context);
    if (field_context == NULL) {
        LOGE("Can't find SerialPort.mNativeContext");
        return -1;
    }
	
    return JNI_TRUE;
}

/*
* Register native methods for all classes we know about.
*
* returns JNI_TRUE on success.
*/
static int registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,
                 method_table, sizeof(method_table) / sizeof(method_table[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}


// ----------------------------------------------------------------------------

/*
* This is called by the VM when the shared library is first loaded.
*/

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    LOGI("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
        LOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;
}

#else
int register_android_hardware_SerialPort(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/hardware/SerialPort/SerialPort");
    if (clazz == NULL) {
        LOGE("Can't find com/hardware/SerialPort/SerialPort");
        return -1;
    }
    field_context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (field_context == NULL) {
        LOGE("Can't find SerialPort.mNativeContext");
        return -1;
    }

    return AndroidRuntime::registerNativeMethods(env, "com/hardware/SerialPort/SerialPort",
            method_table, NELEM(method_table));
}
#endif

