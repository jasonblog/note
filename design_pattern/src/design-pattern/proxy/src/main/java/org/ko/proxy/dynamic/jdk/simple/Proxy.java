package org.ko.proxy.dynamic.jdk.simple;

import org.apache.commons.io.FileUtils;

import javax.tools.JavaCompiler;
import javax.tools.JavaCompiler.CompilationTask;
import javax.tools.StandardJavaFileManager;
import javax.tools.ToolProvider;
import java.io.File;
import java.lang.reflect.Method;

import static java.nio.charset.StandardCharsets.UTF_8;

public class Proxy {
	
	public static Object newProxyInstance(Class clazz,InvocationHandler handler) {
		String methods = "";
		for (Method method : clazz.getMethods()) {
			methods +=
			"	@Override\n" +
			"	public void " + method.getName() + "() {\n" +
			"		try {\n" +
			"			Method method = " + clazz.getName() + ".class.getMethod(\"" + method.getName() + "\");\n" +
			"  			handler.invoke(this, method);\n" +
			"  		} catch (Exception e) {\n" +
			"			e.printStackTrace();\n" +
			"		}\n" +
			"	}\n";
		}
		
		String str =
		"package org.ko.designPattern.proxy.jdk.simple;\n\n" +
		"import java.lang.reflect.Method;\n" +
		"import org.ko.designPattern.proxy.jdk.simple.InvocationHandler;\n\n\n" +
		"public class $Proxy0 implements " + clazz.getName() + " {\n\n" +
		"  	private InvocationHandler handler;\n\n" +
		"	public $Proxy0(InvocationHandler handler) {\n" +
		"		this.handler = handler;\n" +
		"	}\n\n" +
			methods +
		"}";

		try {
			//�����������java�ļ�
			//E:\learn\java\freamwork\ko-base\ko-designPattern\target\classes\org\ko\designPattern\proxy\jdk\simple
			String filename = System.getProperty("user.dir") +"/ko-designPattern/target/classes/org/ko/designPattern/proxy/jdk/simple/$Proxy0.java";
			System.out.println(filename);
			File file = new File(filename);
			FileUtils.writeStringToFile(file, str, UTF_8);

			//����
			//�õ�������
			JavaCompiler complier = ToolProvider.getSystemJavaCompiler();
			//�ļ�������
			StandardJavaFileManager fileManager =
					complier.getStandardFileManager(null, null, null);
			//��ȡ�ļ�
			Iterable units = fileManager.getJavaFileObjects(filename);
			//��������
			CompilationTask task = complier.getTask(null, fileManager, null, null, null, units);
			//���б���
			task.call();

			//�ر�fileManage
			fileManager.close();

			//load ���ڴ�
			ClassLoader cl = ClassLoader.getSystemClassLoader();

			//��ȡclass����
			Class clz = cl.loadClass("org.ko.designPattern.proxy.jdk.simple.$Proxy0");

			//ͨ���������������󲢷���
			return clz.getConstructor(InvocationHandler.class).newInstance(handler);

		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	
	
	
}
