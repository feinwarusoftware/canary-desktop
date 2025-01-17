import React, { useState, useEffect } from 'react';
import { PlayCircle, PauseCircle } from 'react-feather';

function PlayButton({playerObject}){
    /*eslint-disable*/
    useEffect(() => {
        playerObject.updatePlaying.connect(setPlaying);

        return() =>{
            playerObject.updatePlaying.disconnect(setPlaying);
        }
    });
    /*eslint-enable */

    const [i, setPlaying] = useState(false);

    function playCall(){
        playerObject.playerClass.playing().then((isPlaying)=>{
            if(isPlaying){
                playerObject.playerClass.pause()
            }
            else{
                playerObject.playerClass.play()
            }
        })
    }

    return(<div onClick={()=> playCall()}>
        {i ? <PauseCircle size='50'/> : <PlayCircle size='50'/>}
    </div>);
}

export default PlayButton;