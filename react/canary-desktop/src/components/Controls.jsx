import React, { useState } from 'react';
import {toMSS} from './UsefulFunctions';
import '../styles/Controls.css'

function Controls(props){
    const playerObject = props.playerObject;

    const [cpos, setpos] = useState(0);
    const [nowPlayingData, setNowPlayingData] = useState({
        len:0
    });

    //THIS IS BROKEN: WELL BE FIXED ASAP
    /*playerObject.setNowPlayingPos.connect(function(pos) {
        console.log(pos);
        setpos(pos);
    });*/
    
    playerObject.setNowPlayingInfo.connect(setNowPlayingData);

    return(
        <div className="playbar">
            <button onClick={props.stuff}>Load Test Song</button>
            <div className="current-song-info">
                <div className="cover-container">
                    <img alt="Current Song Album Cover" src={nowPlayingData.coverUri} />
                </div>
                <div className="nowplaying">
                    <span>{nowPlayingData.title}</span>
                    <span>{nowPlayingData.artist}</span>
                </div>
            </div>

            <div className="btns">
                <button onClick={() => playerObject.jump("previous")}>Previous</button>
                <button onClick={props.playCall}>Play/Pause</button>
                <button onClick={() => playerObject.jump("forward")}>Forward</button>
            </div>

            <input className="progress-bar" type="range" min="0" /*value={cpos}*/ 
            onChange={(e)=>{
                document.querySelector(".currentTime").innerHTML = toMSS(e.target.value);
            }}
            //style={{backgroundSize:`${cpos * 100 / nowPlayingData.len}% 20px`}}
            max={nowPlayingData.len} 
            onMouseDown={()=>{
                playerObject.isDraggingSeekbar = true;
            }} 
            onMouseUp={
                (e)=>{
                    playerObject.playerClass.seek(e.target.value);
                    playerObject.isDraggingSeekbar = false;
                }
            }/> 
            <div className="time">
                <span className="currentTime"></span>/{toMSS(nowPlayingData.len)}
            </div>
            <i className="fas fa-volume-up"></i> <input className="vs" type="range" defaultValue="1" min="0" max="1" step="any" onChange={(e)=>{playerObject.playerClass.changeVolume(e.target.value)}}/>
        </div>
    );
}

export default Controls;