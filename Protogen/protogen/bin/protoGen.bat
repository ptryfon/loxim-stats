
set APP_HOME "."

$JAVA_HOME\bin\java -classpath $APP_HOME\classworlds-1.1.jar  -Dclassworlds.conf=$APP_HOME\classworlds.conf  -Dapp.home=$APP_HOME   org.codehaus.classworlds.Launcher    $*

