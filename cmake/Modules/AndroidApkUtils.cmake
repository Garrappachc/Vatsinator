#
#       AndroidApkUtils
#
# Functions to help deploy C and C++ applications for Android with
# some Qt goodies.
# 
# The following functions are provided by this module:
# 
#       android_qt_generate_manifest
#

# find sdk utils
find_program(ANT_BIN NAMES ant CMAKE_FIND_ROOT_PATH_BOTH)
if (NOT ANT_BIN)
    message(FATAL_ERROR "Could not find ant. Please install ant and add it to "
        "the search path or provide CMake with the ANT_BIN variable directly.")
endif ()

find_program(ANDROID_BIN NAMES android android.bat CMAKE_FIND_ROOT_PATH_BOTH)
if (NOT ANDROID_BIN)
    message(FATAL_ERROR "Could not find 'Android SDK and AVD Manager'. Please "
        "install the android sdk and add the <android-sdk>/tools directory to "
        "the search path or provide CMake with the ANDROID_BIN variable directly.")
endif ()

find_program(ADB_BIN NAMES adb adb.exe CMAKE_FIND_ROOT_PATH_BOTH)
if (NOT ADB_BIN)
    message(WARNING "Could not find adb. Add it to the path or provide CMake "
        "with the ADB_BIN variable if you want to use the install target")
endif ()

if (Qt5Core_FOUND)
    get_target_property (_qmake_bin Qt5::qmake IMPORTED_LOCATION)
    execute_process(
        COMMAND ${_qmake_bin} -query QT_VERSION
        OUTPUT_VARIABLE _qt_version
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process (
        COMMAND ${_qmake_bin} -query QT_INSTALL_PREFIX
        OUTPUT_VARIABLE _qt_root
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process (
        COMMAND ${_qmake_bin} -query QT_INSTALL_LIBS
        OUTPUT_VARIABLE _qt_libs_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process (
        COMMAND ${_qmake_bin} -query QT_INSTALL_QML
        OUTPUT_VARIABLE _qt_qmls_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process (
        COMMAND ${_qmake_bin} -query QT_INSTALL_PLUGINS
        OUTPUT_VARIABLE _qt_plugins_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

execute_process (
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles
)

# a list of plugins needed for every Android, Qt-based app
set (_android_musthave_plugins
    platforms/android/libqtforandroid.so
)

# a list of necessary jars
set (_android_musthave_jars
    QtAndroid-bundled.jar
)

if (${_qt_version} VERSION_LESS "5.7.0")
    list (APPEND _android_musthave_jars QtAndroidAccessibility-bundled.jar)
endif ()

set(_android_apkutils_dir ${CMAKE_CURRENT_LIST_DIR})
include(CMakeParseArguments)

function (_android_generate_config target)
    cmake_policy (PUSH)
    cmake_policy (SET CMP0026 OLD)
    get_target_property (target_location ${target} LOCATION)
    cmake_policy (POP)
    
    set (libs_xml_location ${_android_apkutils_dir}/libs.xml.in)
    set (files_dir ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles)
    
    get_directory_property(PROJECT_INCLUDES DIRECTORY ${PROJECT_SOURCE_DIR} INCLUDE_DIRECTORIES)
    string(REGEX REPLACE ";" " " PROJECT_INCLUDES "${PROJECT_INCLUDES}")
    set (includes "${PROJECT_INCLUDES}")
    
    configure_file (${_android_apkutils_dir}/AndroidApkUtilsConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsConfig.cmake @ONLY)
endfunction ()


#
#       android_deploy_apk(<target>
#               [RESOURCES resources...]
#               [SOURCES sources...]
#               [PATHS paths...]
#               [QT_QML_MODULES modules...]
#               [QT_PLUGINS plugins...]
#               [QT_GENERATE_LIBS_XML 0|1]
#               [ASSETS_PREFIX prefix]
#               [QML_PLUGINS targets...])
#
# Copies resources, java sources, finds dependencies, installs QML modules,
# plugins and builds the apk package.
#
# This function requries the ANDROID_MANIFEST property of the given target to
# define the path to the AndroidManifest.xml file. The android_generate_manifest
# target sets the property automatically.
#
# The RESOURCES is a list of directories to be copied into res/ subfolder in
# the apk structure.
#
# The SOURCES param is a list of additional Java sources; they will be copied
# into src/ subfolder.
#
# The PATHS is a list of additional paths where the script should be looking
# for dependencies. Qt paths are added by default.
#
# QT_QML_MODULES defines the list of QML modules to be installed; the 
# modules are listed as directories, relative to the qml/ directory in Qt root.
#
# QT_PLUGINS is a list of Qt plugins to be shipped with the apk package,
# relative to the plugins/ directory in the Qt root. Note that all necessary
# plugins (like qtforandroid) are deployed automatically.
#
# The QT_GENERATE_LIBS_XML is enabled by default. If enabled, the libs.xml
# file will be generated and put in res/values/ directory in the apk. It is
# necessary for Qt-based apps.
#
# ASSETS_PREFIX defines the prefix for files installed by AndroidApkUtils in
# the assets/ directory. androiddeployqt uses "--Added-by-androiddeployqt--",
# AndroidApkUtils uses "--Added-by-AndroidApkUtils--" by default.
#
# QML_PLUGINS is a list of targets that are QML plugins. In order to include
# the qmldir file as well, set the QMLDIR_FILE property for each of these
# targets.
#
# PLUGINS is a list of additional libraries to be shipped with the apk.
#
function (android_deploy_apk target)
    cmake_parse_arguments (
        _arg
        ""
        "ASSETS_PREFIX"
        "PATHS;RESOURCES;SOURCES;QT_QML_MODULES;QT_PLUGINS;QT_GENERATE_LIBS_XML;QML_PLUGINS;PLUGINS"
        ${ARGN}
    )
    
    get_target_property (manifest_file ${target} ANDROID_MANIFEST)
    if (NOT manifest_file)
        message (FATAL_ERROR "AndroidManifest.xml not created!")
    endif ()
    
    get_target_property (package_name ${target} ANDROID_PACKAGE_NAME)
    if (NOT package_name)
        message (FATAL_ERROR "Android package name not set! Please update the ANDROID_PACKAGE_NAME property for target ${target}")
    endif ()
    
    set (package_location ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles/${package_name})
    file (MAKE_DIRECTORY ${package_location})
    
    # copy package resources
    file (COPY "${_qt_root}/src/android/java/res" DESTINATION ${package_location})
    
    if (_arg_RESOURCES)
        file (COPY ${_arg_RESOURCES} DESTINATION ${package_location}/res)
    endif ()
    
    # copy java sources
    file (COPY "${_qt_root}/src/android/java/src" DESTINATION ${package_location})
    
    if (_arg_SOURCES)
        file (COPY ${_arg_SOURCES} DESTINATION ${package_location}/src)
    endif ()
    
    # generate build.xml, depends only on AndroidManifest.xml
    add_custom_command (
        OUTPUT ${package_location}/build.xml
        DEPENDS ${manifest_file}
        COMMAND ${CMAKE_COMMAND} -E copy ${manifest_file} ${package_location}
        COMMAND ${ANDROID_BIN} update project --target android-${ANDROID_NATIVE_API_LEVEL} --name ${CMAKE_PROJECT_NAME} --path ${package_location}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles
    )
    
    if (_arg_PATHS)
        set (paths ${_arg_PATHS}) # used in AndroidApkUtilsConfig
    endif ()
    
    set (qt_generate_libs_xml 1) # used in AndroidApkUtilsConfig
    if (NOT "${_arg_QT_GENERATE_LIBS_XML}" STREQUAL "")
        if ("${_arg_QT_GENERATE_LIBS_XML}" MATCHES "^[01]$")
            set (qt_generate_libs_xml ${_arg_QT_GENERATE_LIBS_XML})
         endif ()
    endif ()
    
    if (_arg_QT_QML_MODULES)
        set (qt_qml_modules ${_arg_QT_QML_MODULES}) # used in AndroidApkUtilsConfig
    endif ()
    
    set (qt_plugins ${_android_musthave_plugins}) # used in AndroidApkUtilsConfig
    if (_arg_QT_PLUGINS)
        foreach (p ${_arg_QT_PLUGINS})
            list (APPEND qt_plugins ${p})
        endforeach ()
    endif ()
    list (REMOVE_DUPLICATES qt_plugins)
    
    if (_arg_PLUGINS)
        foreach (p ${_arg_PLUGINS})
            add_dependencies (${target} ${p})
        
            cmake_policy (PUSH)
            cmake_policy (SET CMP0026 OLD)
            get_target_property (p_location ${p} LOCATION)
            cmake_policy (POP)
            
            list (APPEND plugins ${p_location}) # used in AndroidApkUtilsConfig
        endforeach ()
    endif ()
    
    set (qt_jars ${_android_musthave_jars}) # used in AndroidApkUtilsConfig
    list (REMOVE_DUPLICATES qt_jars)
    
    set (assets_prefix "--Added-by-AndroidApkUtils--") # used in AndroidApkUtilsConfig
    if (_arg_ASSETS_PREFIX)
        set (assets_prefix ${_arg_ASSETS_PREFIX})
    endif ()
    
    if (_arg_QML_PLUGINS)
        foreach (p ${_arg_QML_PLUGINS})
            get_target_property (p_dir ${p} PLUGIN_DIR)
            get_target_property (p_qmldir ${p} QMLDIR_FILE)
            get_target_property (p_files ${p} FILES)
            get_target_property (p_prefix ${p} PLUGIN_PREFIX)
            get_filename_component (p_path ${p_qmldir} DIRECTORY)
            
            cmake_policy (PUSH)
            cmake_policy (SET CMP0026 OLD)
            get_target_property (p_location ${p} LOCATION)
            cmake_policy (POP)
            
            list (APPEND qml_plugins ${p})
            
            set (qml_plugins_data "${qml_plugins_data}
                set (${p}_dir \"${p_dir}\")
                set (${p}_qmldir \"${p_qmldir}\")")
            
            if (p_location)
                set (qml_plugins_data "${qml_plugins_data}
                    set (${p}_location \"${p_location}\")")
            endif ()
            
            if (p_files)
                set (qml_plugins_data "${qml_plugins_data}
                    set (${p}_files \"${p_files}\")")

                # add to list of all qml files so that apk is dependent on them
                foreach (f ${p_files})
                    list (APPEND all_files ${p_path}/${f})
                endforeach ()
            endif()
            
            if (p_prefix)
                set (qml_plugins_data "${qml_plugins_data}
                    set (${p}_prefix \"${p_prefix}\")")
            else ()
                set (qml_plugins_data "${qml_plugins_data}
                    set (${p}_prefix \"qml\")")
            endif ()
        endforeach ()
    endif ()
    
    # used in AndroidApkUtilsConfig
    get_target_property (keystore ${target} KEYSTORE)
    get_target_property (keystore_alias ${target} KEYSTORE_ALIAS)
    get_target_property (keystore_password ${target} KEYSTORE_PASSWORD)
    
    _android_generate_config(${target})
    configure_file (${_android_apkutils_dir}/AndroidApkUtilsDeploy.cmake ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsDeploy.cmake COPYONLY)

    string (TOLOWER ${CMAKE_PROJECT_NAME} app_name)
    if (keystore AND keystore_alias AND keystore_password)
        set (package_file_name ${app_name}-release.apk)
    else ()
        set (package_file_name ${app_name}-debug.apk)
    endif ()

    # builds the apk itself
    add_custom_command (
        OUTPUT ${package_file_name} # eg. fooapp-debug.apk
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsDeploy.cmake
        DEPENDS
            ${package_location}/build.xml   # when build.xml changes
            ${_arg_QML_PLUGINS}             # when any of the qml plugins is rebuilt
            ${_arg_PLUGINS}                 # when any of the application plugins is rebuilt
            ${all_files}                    # i.e. when qml file is changed
            ${target}                       # when the application is rebuilt
    )
    
    add_custom_target (apk ALL
        DEPENDS ${package_file_name}
    )
    
    add_custom_target (apk_install
        DEPENDS apk
        COMMAND ${ADB_BIN} install -r ${package_file_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    get_target_property (package_name ${target} ANDROID_PACKAGE_NAME)
    get_target_property (package_activity ${target} ANDROID_ACTIVITY_CLASS)
    
    if (package_name AND package_activity)
        add_custom_target (apk_run
            DEPENDS apk
            COMMAND ${ADB_BIN} shell am start -n ${package_name}/${package_activity}
        )
    endif ()
endfunction ()

#
#       android_generate_manifest(<output>
#               [TEMPLATE_FILE template_file]
#               [ANDROID_APPLICATION_CLASS application_class]
#               [ANDROID_ACTIVITY_CLASS activity_class]
#               [ANDROID_PERMISSIONS permissions...]
#               [QT_BUNDLE_LOCAL_QT_LIBS 0|1])
#               
# Generates the AndroidManifest.xml file from the given template. The template
# is taken from Qt sources and is designed to support Qt applications. It is
# however possible to provide a custom template.
#
# The TEMPLATE_FILE is a path to the AndroidManifest.xml template file. If not
# specified, the Qt's default template is used.
#
# The ANDROID_APPLICATION_CLASS param can be set to define the value of "android:name"
# param in the XML. If not set, "org.qtproject.qt5.android.bindings.QtApplication" is
# used, as it is the default Application implementation for Qt.
#
# The ANDROID_ACTIVITY_CLASS defines the class that implements the main activity.
# If not set, "org.qtproject.qt5.android.bindings.QtActivity" is used.
#
# The ANDROID_PERMISSIONS is a list of Android-specific permissions that will be put
# in the manifest file.
#
# The QT_BUNDLE_LOCAL_QT_LIBS specifies whether the Qt libs should be bundled with the
# APK or not. By default this value is set to 1, meaning that the apk does not
# need Ministro.
#
# As a result, the ANDROID_MANIFEST property on the specified target is set, defining
# the absolute location of the generated AndroidManifest.xml file.
#
# Sample usage:
# android_qt_generate_manifest(baz
#               ANDROID_APP_LIB "baz"
#               ANDROID_ACTIVITY_CLASS "org.foo.bar.BazActivity"
#               ANDROID_PERMISSIONS ACCESS_NETWORK_STATE INTERNET RECEIVE_SMS)
#
# TODO Add ANDROID_FEATURES option to auto-generate features as well
#
function (android_generate_manifest target)
    cmake_parse_arguments (
        _arg
        ""
        "TEMPLATE_FILE;ANDROID_APPLICATION_CLASS;ANDROID_ACTIVITY_CLASS;QT_BUNDLE_LOCAL_QT_LIBS"
        "ANDROID_PERMISSIONS"
        ${ARGN}
    )
    
    get_target_property (ANDROID_APP_NAME ${target} ANDROID_APP_NAME)
    if (NOT ANDROID_APP_NAME)
        message (FATAL_ERROR "Android app name not set! Please update the ANDROID_APP_NAME property for target ${target}")
    endif ()
    
    get_target_property (ANDROID_VERSION_NAME ${target} ANDROID_VERSION_NAME)
    if (NOT ANDROID_VERSION_NAME)
        message (FATAL_ERROR "Android app version name not set! Please update the ANDROID_VERSION_NAME property for target ${target}")
    endif ()
    
    get_target_property (ANDROID_VERSION_CODE ${target} ANDROID_VERSION_CODE)
    if (NOT ANDROID_VERSION_CODE)
        message (FATAL_ERROR "Android app version code not set! Please update the ANDROID_VERSION_CODE property for target ${target}")
    endif ()
    
    get_target_property (ANDROID_PACKAGE_NAME ${target} ANDROID_PACKAGE_NAME)
    if (NOT ANDROID_PACKAGE_NAME)
        message (FATAL_ERROR "Android package name not set! Please update the ANDROID_PACKAGE_NAME property for target ${target}")
    endif ()
    
    set (ANDROID_APP_LIB ${target}) # TODO make this customizable
    
    set (TEMPLATE_FILE "${_android_apkutils_dir}/AndroidManifest.xml.in")
    if (_arg_TEMPLATE_FILE)
        set (TEMPLATE_FILE ${_arg_TEMPLATE_FILE})
    endif ()
    
    set (ANDROID_APPLICATION_CLASS "org.qtproject.qt5.android.bindings.QtApplication")
    if (_arg_ANDROID_APPLICATION_CLASS)
        set (ANDROID_APPLICATION_CLASS ${_arg_ANDROID_APPLICATION_CLASS})
    endif ()
    
    set (ANDROID_ACTIVITY_CLASS "org.qtproject.qt5.android.bindings.QtActivity")
    if (_arg_ANDROID_ACTIVITY_CLASS)
        set (ANDROID_ACTIVITY_CLASS ${_arg_ANDROID_ACTIVITY_CLASS})
    endif ()
    
    set_target_properties (${target} PROPERTIES ANDROID_ACTIVITY_CLASS ${ANDROID_ACTIVITY_CLASS})
    
    if (_arg_ANDROID_PERMISSIONS)
        set (ANDROID_PERMISSIONS "")
        foreach (p ${_arg_ANDROID_PERMISSIONS})
            set (ANDROID_PERMISSIONS "${ANDROID_PERMISSIONS}\n    <uses-permission android:name=\"android.permission.${p}\" />")
        endforeach ()
    endif ()
    
    set (QT_BUNDLE_LOCAL_QT_LIBS 1)
    if (NOT "${_arg_QT_BUNDLE_LOCAL_QT_LIBS}" STREQUAL "")
        if ("${_arg_QT_BUNDLE_LOCAL_QT_LIBS}" MATCHES "^[01]$")
            set (QT_BUNDLE_LOCAL_QT_LIBS ${_arg_QT_BUNDLE_LOCAL_QT_LIBS})
        endif ()
    endif ()
    
    set (output "${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles/AndroidManifest.xml")
    configure_file (${TEMPLATE_FILE} ${output} @ONLY)
    set_target_properties (${target} PROPERTIES ANDROID_MANIFEST ${output})
endfunction ()
