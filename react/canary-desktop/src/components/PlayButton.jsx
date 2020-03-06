import React, { useState, useEffect } from 'react';
import { PlayCircle, PauseCircle } from 'react-feather';

function PlayButton({playerObject}){
    useEffect(() => {
        playerObject.updatePlaying.connect(setPlaying);

        return() =>{
            playerObject.updatePlaying.disconnect(setPlaying);
        }
    },  []);

    const [i, setPlaying] = useState(false);

    function playCall(){
        playerObject.playerClass.playing().then((isPlaying)=>{
            if(isPlaying){
                setPlaying(false);
                playerObject.playerClass.pause()
            }
            else{
                setPlaying(true);
                playerObject.playerClass.play()
            }
        })
    }

    return(<div onClick={()=> playCall()}>
        {i ? <PauseCircle size='50'/> : <PlayCircle size='50'/>}
    </div>);
}

export default PlayButton;