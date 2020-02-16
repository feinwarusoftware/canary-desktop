import React, { useEffect } from 'react';

function Queue(props){
    const playerObject = props.playerObject;

    function getQueueFunc(){
        return playerObject.playerClass.getQueue().then((queue)=>{
            console.log(queue);
        });
    }

    useEffect(() => {
        playerObject.queueUpdate.connect(getQueueFunc);

        return() =>{
            playerObject.queueUpdate.disconnect(getQueueFunc);
        }
    });

    getQueueFunc();

    return(
        <div>
            oi eu sou o queue
        </div>
    );
}

export default Queue;