import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
//import com.client.emindprint 1.0
import "client.js" as Jsclient
import QtQuick.Dialogs 1.2


Item {
    objectName: "ClientWin"
    id:clientWin
    width: 400
    height: 240
    //    title: qsTr("EmindCloudPrinter")
    //    Height: 240
    //    minimumHeight: 240
    //    maximumWidth: 400
    //    Width: 400
    visible:true

    //    onVisibleChanged: console.log("Clientlogin window visible changed")
    property int reqok:0
    property int reqAuth:0

    signal showPrinterWin()


    PrinterList{
        id:pWin
        objectName: "PrinterListWin"
    }

    //    Connections{
    //        target:clientWin
    //        onShowPrinterWin:{
    //            pWin.visible = true;
    //        }
    //    }


//    //clientAssistant{
//        id://clientAssist
//        onSigConnected: {
//            //            clientWin.showPrinterWin();
//            //            clientWin.hide();
//        }
//        onSigAuthWrong:{
//            errText2.color="red";
//            errText2.visible = true;
//            fieldLicense.style = fieldStyleErr;
//        }
//        onSigConnectRefused: {
//            errText1.visible = true;
//            errText1.color="red";
//            fieldIP.style=fieldStyleErr;
//        }
//        onPlistSent:{
//            //            Jsclient.g_str = client.pnameStr;
//            Jsclient.g_str = //clientAssist.pnameStr;
//            console.log("ClientLogin.qml:",Jsclient.g_str);
//            //            console.log(Jsclient.g_str);
//            pWin.showList();
//        }
//        onPrinterSetFinished:{
//            pWin.stopSpinner();
//        }
//        //        onStopIndicator:{
//        //            pWin.stopSpinner();
//        //        }

//        //        onSigDefaultPrinterDone: {
//        //            doneMsg.show(qsTr("The Remote Printer Server Has Set The Default Printer Successfully!\n You May Start to Print Your Papers!"));
//        //        }
//        //        onSigDefaultPrinterErr: errMsg.show(qsTr("The Remote Printer Server Set The Default Printer Failed!\n
//        //Please Check You Server!"));

//        onSigPrintSuccess:{
//            msgDialog.title = qsTr("EmindCloudPrinter");
//            msgDialog.text = qsTr("Printed Success!");
//            msgDialog.visible = true;
//        }
//        onSigPrintFailed: {
//            console.log("onSigPrintFailed");
//            msgDialog.title = qsTr("EmindCloudPrinter");
//            msgDialog.text = qsTr("Printed Failed!");
//            msgDialog.visible = true;
//        }

        //        onSigShowWin:{
        //            if(Jsclient.g_str)
        //            {
        //                pWin.visible = true;
        //                console.log("reshow window pWin");
        //            }
        //            else
        //            {
        //                clientWin.visible = true;
        //                console.log("reshow window clientWin");
        //            }
        //        }

        //        Component.onCompleted: console.log("//clientAssistant born");
//    }

    MessageDialog{
        id:msgDialog
    }



    Component {
        id:fieldStyleErr
        TextFieldStyle{
            background: Rectangle{
                border.color: "#ff0000"
            }
        }
    }

    Component {
        id:fieldStyleFocus
        TextFieldStyle {
            background: Rectangle {
                border.color: "#0087ff"
                border.width: 1
            }
        }
    }
    Component {
        id:fieldStyleNormal
        TextFieldStyle {
            background: Rectangle {
                border.color: "#999999"
                border.width: 1
            }
        }
    }

    Button {
        objectName:"btnCancel"
        id: btnCancel1
        x: 88
        y: 173
        width: 103
        height: 36
        style: ButtonStyle {
            background: Rectangle{
                radius: 5
                border.color: "#cccccc"
                border.width: 1
                color: (control.hovered || control.focus)? "#0087ff":"#f5f5f5"
            }
        }

        text: qsTr("Cancel")

        onClicked:
        {
            Qt.quit();
        }
        Keys.onPressed: {
            if((event.key === Qt.Key_Enter) || (event.key === Qt.Key_Return))
                btnCancel1.clicked();
        }
        activeFocusOnPress:true
    }



    Button {
        objectName:"btnNext"
        id: btnNext
        x: 210
        y: 173
        width: 103
        height: 36
        //        activeFocusOnPress:true
        KeyNavigation.tab: btnCancel1

        style: ButtonStyle {
            background: Rectangle{
                radius: 5
                border.color: "#cccccc"
                border.width: 1
                color: (control.hovered || control.focus)? "#0087ff":"#f5f5f5"
            }
        }

        Keys.onPressed: {
            if((event.key === Qt.Key_Enter) || (event.key === Qt.Key_Return))
                btnNext.clicked();
        }

        text: qsTr("Next")

        enabled: enableBtnNext()


        onClicked:
        {
            btnNext.enabled = false;

            //clientAssist.checkConnectivity(fieldIP.text,fieldLicense.text);
            console.log("next button pressed");
            btnNext.enabled = true;
        }
    }

    function enableBtnNext(){
        if((fieldIP.text !== "") && (fieldLicense.text !== "")){
            return true;
        }else{
            return false;
        }
    }

    Text {
        id: text1
        x: 62
        y: 49
        width: 119
        height: 29
        opacity: 0.9
        font.pixelSize: 14
        text:qsTr("Server Address:")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }

    Text {
        id: text2
        x: 65
        y: 113
        width: 119
        height: 30
        font.pixelSize: 14
        text:qsTr("  License:")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }

    Text {
        objectName: "errText1"
        id: errText1
        x: 158
        y: 79
        width: 182
        height: 28
        text: qsTr("Connect failed")
        visible: false
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        //        wrapMode: Text.WordWrap
        font.pixelSize: 14

    }

    Text {
        objectName: "errText2"
        id: errText2
        x: 158
        y: 142
        width: 182
        height: 31
        text: qsTr("License error")
        visible: false
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 14
    }


    TextField {
        id: fieldIP
        x: 202
        y: 50
        width: 120
        height: 30
        font.pixelSize: 12
        placeholderText: " " //clientAssist.serverip
        focus: false
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        validator: RegExpValidator{
            regExp: /\d{1,3}[.]\d{1,3}[.]\d{1,3}[.]\d{1,3}/
        }

        style:fieldStyleNormal
//        activeFocusOnPress:true
//        selectByMouse: true
//        onSelectByMouseChanged: fieldIP.style = fieldStyleFocus;
        onFocusChanged: {
            if(fieldIP.focus == true)
                fieldIP.style = fieldStyleFocus;
            else
                fieldIP.style = fieldStyleNormal;
        }
        //      inputMask: "DDD.DDD.DDD.DDD;"
        //        ToolTips{
        //            id: tooltip1
        //             width: 150
        //             y:parent.y-5
        //             target: fieldIP
        //             text: qsTr("Enter IP like:192.168.0.1")
        //             z:999
        //        }
        //        MouseArea{
        //            anchors.fill: parent
        //            onEntered: label1.visible = true;
        //        }
    }

    TextField {
        id: fieldLicense
        x: 203
        y: 112
        width: 120
        height: 30
        font.pixelSize: 12
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        placeholderText: " " //clientAssist.license
        KeyNavigation.tab: btnNext
        focus:false
        style: fieldStyleNormal
        onFocusChanged: {
            if(fieldLicense.focus == true)
                fieldLicense.style = fieldStyleFocus;
            else
                fieldLicense.style = fieldStyleNormal;
        }
    }

    //    Label {
    //        id: label1
    //        x: 202
    //        y: 18
    //        width: 121
    //        height: 26
    //        text: qsTr("Enter IP like:192.168.0.1")
    //        visible:false
    //        Behavior on visible { NumberAnimation { duration: 100 }}
    //        z:99999
    //    }




}
