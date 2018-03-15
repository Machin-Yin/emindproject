WorkerScript.onMessage = function(msg) {
    console.log(msg.index,msg.prstate);
    for(var i=0;i<msg.prname.length;i++){
        msg.model.append({"prname":msg.prname[i],"prstate": msg.buttonText});
    }

    msg.model.sync();
}
