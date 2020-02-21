import React, { useState, useEffect } from 'react';
import {toMSS} from './UsefulFunctions';
import '../styles/Controls.css'

function Controls(props){
    const playerObject = props.playerObject;

    const [nowPlayingData, setNowPlayingData] = useState({
        len:0
    });

    const [nowPlayingSongPos, setPos] = useState(0);

    useEffect(() => {
        playerObject.setNowPlayingInfo.connect(setNowPlayingData); //TODO: fix weird seekbar when song transition thorugh here

        playerObject.clear.connect(function(){
            setNowPlayingData({
                len:0,
                title:"",
                artist:"",
                album:"",
                coverUri:""
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
                <button onClick={()=>{
                    playerObject.insertToQueue(1, "G:/Músicas/Rainbow/Rising/05. Stargazer.flac");
                    playerObject.insertToQueue(2, "G:/Músicas/Ok Goodnight/Limbo/Ok Goodnight - Limbo - 03 Free Fall.flac");
                }}>Load more</button>
            </div>

            <input className="progress-bar" type="range" min="0"
            value={nowPlayingSongPos}
            onChange={(e)=>{
                setPos(e.target.value);
            }}
            max={nowPlayingData.len} 
            onMouseDown={()=>{
                console.log("mousedown")
                playerObject.isDraggingSeekbar = true;
            }} 

            /> 
            <div className="time">
                <span className="currentTime">{toMSS(nowPlayingSongPos)}</span>/{toMSS(nowPlayingData.len)}
            </div>
            <i className="fas fa-volume-up"></i> <input className="vs" type="range" defaultValue="1" min="0" max="1" step="any" onChange={(e)=>{playerObject.playerClass.changeVolume(e.target.value)}}/>
        </div>
    );
}

export default Controls;