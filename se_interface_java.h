#ifndef __SE_INTERFACE_JAVA_H__
#define __SE_INTERFACE_JAVA_H__
#ifdef _FOR_JAVA
#define java_interface(s) Java_compiler_##s
extern "C" {
/*
 * Class:     compiler
 * Method:    SetDbgCmdID
 * Signature: (J)V
 */
JNIEXPORT void JNICALL java_interface(SE_1SetDbgCmd)
  (JNIEnv *, jclass, jlong, jlong, jlong, jstring);


/*
 * Class:     hello
 * Method:    SE_compile
 * Signature: ([C)J
 */
JNIEXPORT jlong JNICALL java_interface(SE_1compile)
  (JNIEnv *, jclass, jstring);

/*
 * Class:     hello
 * Method:    SE_HandleRequest
 * Signature: ([B[BJ)I
 */
JNIEXPORT jint JNICALL java_interface(SE_1HandleRequest)
  (JNIEnv *env, jclass, jstring , jlong , jlong );

/*
 * Class:     hello
 * Method:    SE_RecompileAll
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL java_interface(SE_1RecompileAll)
  (JNIEnv *, jclass);


/*
 * Class:     hello
 * Method:    SE_SetDebugInput
 * Signature: (J)V
 */
JNIEXPORT void JNICALL java_interface(SE_1SetDebugInput)
  (JNIEnv *, jclass, jlong);

/*
 * Class:     hello
 * Method:    SE_GetVersion
 * Signature: ([CJ)V
 */
JNIEXPORT void JNICALL java_interface(SE_1GetVersion)
  (JNIEnv *, jclass, jstring, jlong);

/*
 * Class:     hello
 * Method:    SE_LoadFunctionFile
 * Signature: ([C[C)J
 */
JNIEXPORT jlong JNICALL java_interface(SE_1LoadFunctionFile)
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     hello
 * Method:    SE_Start
 * Signature: ([CJI)I
 */
JNIEXPORT jint JNICALL java_interface(SE_1Start)
  (JNIEnv *, jclass, jstring, jlong, jint);

JNIEXPORT void JNICALL java_interface(SE_1SetLog)
  (JNIEnv *env, jclass, jlong);

JNIEXPORT jstring JNICALL Java_compiler_SE_1GetErrFileName
  (JNIEnv *, jclass);

JNIEXPORT jlong JNICALL java_interface(SE_1GetDbgOut)
  (JNIEnv *env, jclass, jstring msg);

JNIEXPORT jlong JNICALL java_interface(SE_1GetDebugVMReg)
  (JNIEnv *env, jclass, jlong lIndex);

JNIEXPORT void JNICALL java_interface(SE_1GetPFunctionTable)
 (JNIEnv *env, jclass cls, jstring );

JNIEXPORT jlong JNICALL java_interface(SE_1IsDebugBreak)
  (JNIEnv *env,jclass);

JNIEXPORT void JNICALL java_interface(SE_1MemToFile)
  (JNIEnv *env,jclass,jstring fname);

JNIEXPORT jlong JNICALL java_interface(SE_1GetCurLine)
  (JNIEnv *env, jclass cls);

JNIEXPORT jstring JNICALL java_interface(SE_1GetCurFuncName)
  (JNIEnv *env, jclass cls);
}

#endif // _FOR_JAVA

#endif //__SE_INTERFACE_JAVA_H__
