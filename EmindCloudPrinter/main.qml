import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
//import com.client.emindprint 1.0
import "client.js" as Jsclient
import QtQuick.Dialogs 1.2

ApplicationWindow {
    objectName: "rootObj"
    id:root

    //    width: 400
    //    height: 240

    minimumHeight: 240
    minimumWidth: 400
    visible:true
    title: qsTr("EmindCloudPrinter")
    style: ApplicationWindowStyle {
        background:Rectangle{
            color:"white"
        }
    }

    //    Loader{
    //        id:mainWin
    //        objectName: "ClientWindow"
    //    }

    //    Component.onCompleted: {
    //        mainWin.source = "ClientLogin.qml";
    //    }

    Component{
        id:pwin
        PrinterList{
            id:printerwin
            visible: true
            addedPname: clientAssist.pnameStr
            Connections{
                target: clientAssist
                onShowList:{
                    printerwin.showList();
                }

            }
            Connections{
                target: clientAssist
                onPrnAdded:{
                    console.log("onPrnAdded",addedPname);
                    printerwin.cmpPname(addedPname);
                }
            }

        }
    }

//    ClientAssistant{
//        id:clientAssist
//        signal showList()
//        signal prnAdded(var addedPname)
//        onSigConnected: {
//            //            stackView.push({item:Qt.resolvedUrl("qrc:/PrinterList.qml")});
//            stackView.push({item:pwin});
//            root.width = 600;
//            root.height = 500;
//            root.title = qsTr("Add Printer");
//        }
//        onPlistSent:{
//            //            Jsclient.g_str = client.pnameStr;
//            Jsclient.g_str = clientAssist.pnameStr;
//            console.log("ClientLogin.qml:",Jsclient.g_str);
//            //            console.log(Jsclient.g_str);
//            showList();
//        }
//        onSigRunningWarn:{
//            msgDialog.title = qsTr("EmindCloudPrinter");
//            msgDialog.text = qsTr("EmindCloudPrinter Is Running!");
//            msgDialog.visible = true;
//        }
//        onPrinterAdded:{
////            prnAdded(clientAssist.pnameStr);
//            console.log("clientAssist.pnameStr=",clientAssist.pnameStr);
//            prnAdded(clientAssist.pnameStr);
//        }
//        onSigOffline:{
//            stackView.enabled=false;
//            layered.visible = true;
//        }
//        onSigPrnDeleted:{
//            console.log("onSigPrnDeleted");
//            console.log("clientAssist.pnameStr=",clientAssist.pnameStr);
//        }

//    }


    Item {
          id: layered
          opacity: 0.3
          visible: false

          implicitHeight : root.height
          implicitWidth : root.width
          Rectangle { width: root.width; height: root.height; color: "grey"; }
          z:8888
      }


    MessageDialog{
        id:msgDialog
    }



    Component{
        id:cwin
        ClientLogin{
            id:clientwin
            visible: true
        }
    }

    StackView{
        id:stackView
        anchors.fill: parent
        initialItem: {
            item:cwin
            //            "item":Qt.resolvedUrl("qrc:/ClientLogin.qml")
            //            "properties":{"id":"loginPage"}
        }
    }



}
