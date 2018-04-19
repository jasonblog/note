# 簽署你的Android應用程式(.keystore)


產生的方法很簡單，只要照著貼就可以了。


```sh
keytool -genkey -v -keystore rdss.keystore -alias rdss -keyalg RSA -keysize 2048 -validity 10000
```

如果這邊出現「keytool: command not found」，請把$JAVA_HOME\bin加入環境變數$PATH中，或是直接到$JAVA_HOME\bin\目錄下執行keytool，例如小蛙的環境是C:\Program Files\Java\jdk1.7.0_02\bin，就先cd C:\Program Files\Java\jdk1.7.0_02\bin之後開始輸入上面的指令。


將上述指令-keystore rdss.keystore以及-alias rdss中的`rdss`改成自己要的名字，其他東西照著貼上就好，-validity官方說明文件建議設定10000以上的值(指令細節可參考官方文件)。

```sh
輸入金鑰儲存庫密碼: mykeystorepassword
重新輸入新密碼: mykeystorepassword
您的名字與姓氏為何？
  [Unknown]:  Hans
您的組織單位名稱為何？
  [Unknown]:
您的組織名稱為何？
  [Unknown]:
您所在的城市或地區名稱為何？
  [Unknown]:  Taipei
您所在的州及省份名稱為何？
  [Unknown]:  Taiwan
此單位的兩個字母國別代碼為何？
  [Unknown]:  TW
CN=Hans, OU=Unknown, O=Unknown, L=Taipei, ST=Taiwan, C=TW 正確嗎？
  [否]:  Y

針對 CN=Hans, OU=Unknown, O=Unknown, L=Taipei, ST=Taiwan, C=TW 產生有效期 10,000
 天的 2,048 位元 RSA 金鑰組以及自我簽署憑證 (SHA256withRSA)

輸入 <rdss> 的金鑰密碼
        (RETURN 如果和金鑰儲存庫密碼相同):
新憑證 (自我簽署):
...
[儲存 rdss.keystore]
```

密碼千萬要記牢，小蛙在開發照照明星臉的時候就是忘了密碼，花了兩天的時間把所有可能的密碼都試過一次才終於式出來><(前後兩個基本上用一樣的密碼就好了，免得忘記)，完成後可以看到在執行keytool的目錄下出現了一個剛剛設定的「rdss.keystore」，如此一來就完成自己的簽署檔案囉！


主要加入下面兩段

```sh
signingConfigs {                                                                                                                    
	config {
		keyAlias 'rdss'
		keyPassword '123456'
		storeFile file("../rdss.keystore")
		storePassword '123456'
	}
}   

buildTypes {
	release {
		minifyEnabled false
		proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
		signingConfig signingConfigs.config
    }
}
```



```sh
apply plugin: 'com.android.application'

android {
    compileSdkVersion 25
    buildToolsVersion "26.0.2"
    defaultConfig {
        applicationId "com.tengmuz.jnidemo"
        minSdkVersion 17
        targetSdkVersion 25
        versionCode 1
        versionName "1.0"
        testInstrumentationRunner "android.support.test.runner.AndroidJUnitRunner"
    }

	signingConfigs {                                                                                                                    
		config {
			keyAlias 'rdss'
			keyPassword '123456'
			storeFile file("../rdss.keystore")
			storePassword '123456'
		}
	}   

	buildTypes {
		release {
			minifyEnabled false
			proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
			signingConfig signingConfigs.config
        }
    }

    externalNativeBuild {
        ndkBuild {
            path file("src/main/jni/android.mk")
        }
    }
}

dependencies {
    compile fileTree(dir: 'libs', include: ['*.jar'])
    androidTestCompile('com.android.support.test.espresso:espresso-core:2.2.2', {
        exclude group: 'com.android.support', module: 'support-annotations'
    })
    compile 'com.android.support:appcompat-v7:25.3.1'
    compile 'com.android.support.constraint:constraint-layout:1.0.2'
    testCompile 'junit:junit:4.12'
}
```