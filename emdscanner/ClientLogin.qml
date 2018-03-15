import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import com.client.emindprint 1.0
import "client.js" as Jsclient
import QtQuick.Dialogs 1.2


Item {
    objectName: "ClientWin"
    id:clientWin
    width: 400
    height: 240
    visible:true

    Connections{
        target: clienter
        onSigAuthWrong:{
            errText2.color="red";
            errText2.visible = true;
            fieldLicense.style = fieldStyleErr;
        }
    }
    Connections{
        target: clienter
        onSigConnectRefused: {
            errText1.visible = true;
            errText1.color="red";
            fieldIP.style=fieldStyleErr;
        }
    }

    Connections{
        target: clienter
        onPrinterSetFinished:{
            pWin.stopSpinner();
        }
    }

    Connections{
        target: clienter
        onSigPrintSuccess:{
            msgDialog.title = qsTr("EmindCloudPrinter");
            msgDialog.text = qsTr("Printed Success!");
            msgDialog.visible = true;
        }
    }
    Connections{
        target: clienter
        onSigPrintFailed: {
            console.log("onSigPrintFailed");
            msgDialog.title = qsTr("EmindCloudPrinter");
            msgDialog.text = qsTr("Printed Failed!");
            msgDialog.visible = true;
        }

    }



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

        Timer{
            id:timer
            interval: 2000
            onTriggered: btnNext.enabled=true;
        }
        onClicked:
        {
            btnNext.enabled = false;
            Jsclient.g_ip = fieldIP.text;
            clienter.servip = fieldIP.text;
            clienter.sigConnServer(fieldIP.text,fieldLicense.text);
            timer.start();
//            btnNext.enabled = true;
            fieldLicense.focus = false;
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

        onFocusChanged: {
            if(fieldIP.focus == true){
                fieldIP.style = fieldStyleFocus;
                errText1.visible=false;
            }
            else
                fieldIP.style = fieldStyleNormal;
        }

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
        placeholderText: " "
        KeyNavigation.tab: btnNext
        focus:false
        style: fieldStyleNormal
        onFocusChanged: {
            if(fieldLicense.focus == true){
                fieldLicense.style = fieldStyleFocus;
                errText2.visible=false;
            }else
                fieldLicense.style = fieldStyleNormal;
        }

        validator: RegExpValidator{
            regExp: /[\S\d]{1,16}/
        }

    }


}
