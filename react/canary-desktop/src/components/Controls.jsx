import React, { useState, useEffect } from 'react';
import { SkipForward, SkipBack, Volume2 } from 'react-feather';
import PlayButton from './PlayButton';
import {toMSS} from './UsefulFunctions';
import '../styles/Controls.css'

function Controls({playerObject}){
    const [nowPlayingData, setNowPlayingData] = useState(playerObject.nowPlaying);

    const [nowPlayingSongPos, setPos] = useState(0);

    useEffect(() => {
        playerObject.setNowPlayingInfo.connect(setNowPlayingData); //TODO: fix weird seekbar when song transition thorugh here

        playerObject.clear.connect(function(){
            setNowPlayingData({
                lengthInSeconds:0,
                title:"",
                artist:"",
                album:""
            })
        });

        playerObject.setNowPlayingPos.connect(function(pos) {
            setPos(pos);
        });

        return() =>{
            //don't think it's necessary: give names to all callback functions and properly disconnect them here
            playerObject.setNowPlayingInfo.disconnect();
            playerObject.clear.disconnect();
            playerObject.setNowPlayingPos.disconnect();
        }
    },  [playerObject.setNowPlayingInfo, playerObject.clear, playerObject.setNowPlayingPos]);

    return(
        <div className="playbar">
            <div className="current-song-info">
                <div className="cover-container" style={{backgroundImage:`url('${process.env.PUBLIC_URL}/cache/covers/${nowPlayingData.albumid}.jpg')`}}></div>
                <div className="nowplaying">
                    <span class="nameContainer">{nowPlayingData.title}</span>
                    <span class="nameContainer">{nowPlayingData.artist}</span>
                    <span className="time">
                        {toMSS(nowPlayingSongPos)}/{toMSS(nowPlayingData.lengthInSeconds)}
                    </span>
                </div>
            </div>

            <div className="btns">
                <SkipBack size='30' onClick={() => playerObject.jump("previous")}/>
                <PlayButton playerObject={playerObject}/>
                <SkipForward size='30' onClick={() => playerObject.jump("forward")}/>
            </div>

            <input className="progress-bar" type="range" min="0"
            value={nowPlayingSongPos}
            onChange={(e)=>{
                setPos(e.target.value);
            }}
            max={nowPlayingData.lengthInSeconds} 
            onMouseDown={()=>{
                console.log("mousedown")
                playerObject.isDraggingSeekbar = true;
            }} 

            /> 
            <div class="volContainer">
                <Volume2 size='20'/>
                <input className="vs" type="range" defaultValue="1" min="0" max="1" step="any" onChange={(e)=>{playerObject.playerClass.changeVolume(e.target.value)}}/>
            </div>
        </div>
    );
}

export default Controls;