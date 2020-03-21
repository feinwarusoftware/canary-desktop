import React, { useState, useEffect } from 'react';
import { SkipForward, SkipBack, Volume2, VolumeX, Repeat, Shuffle } from 'react-feather';
import ReactStars from 'react-stars'
import PlayButton from './PlayButton';
import {toMSS} from './UsefulFunctions';
import '../styles/Controls.css'

function Controls({playerObject, libObject}){
    const [nowPlayingData, setNowPlayingData] = useState(playerObject.nowPlaying);

    const [nowPlayingSongPos, setPos] = useState(0);

    const [repeat, setRepeatValue] = useState(0);

    const [shuffle, setShuffleValue] = useState(false);

    const [volume, setVolume] = useState({
        isMuted:false,
        previousVolume:1,
        value:1
    });

    function changeRating(newRating){
        console.log('oi');
        libObject.libClass.setRating(newRating * 10, Number(nowPlayingData.index), nowPlayingData).then(()=>{
            libObject.libClass.updateSong(nowPlayingData.index);
        });

        setNowPlayingData({...nowPlayingData, rating:newRating})
    }

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
                    <span className="nameContainer">{nowPlayingData.title}</span>
                    <span className="nameContainer">{nowPlayingData.artist}</span>
                    <span className="time">
                        {toMSS(nowPlayingSongPos)}/{toMSS(nowPlayingData.lengthInSeconds)}
                    </span>
                </div>
            </div>

            <div className="btns">
                <SkipBack size='30' onClick={() => playerObject.jump("previous")}/>
                <PlayButton playerObject={playerObject}/>
                <SkipForward size='30' onClick={() => playerObject.jump("forward")}/>
                <div onClick={()=>{
                        let n = repeat;
                        if(n < 2){
                            n = n + 1;
                        }
                        else{
                            n = 0;
                        }
                        playerObject.playerClass.setRepeat(n); //void function, no callback waiting
                        setRepeatValue(n);
                }} className="repeat">
                    <Repeat size="20" stroke={repeat === 1 || repeat === 2 ? "#ffa800" : "#fff"}/>
                    <span className={repeat === 2 ? "repeat-indicator" : ""}>{repeat === 2 ? "1" :""}</span>
                </div>
                <Shuffle stroke={shuffle ? "#ffa800" : "#fff"} size='30' onClick={()=>{
                    playerObject.playerClass.setShuffle(!shuffle).then((s)=>{
                        setShuffleValue(s); //wait for callbacks when possible, s = !shuffle
                    });
                }}></Shuffle>
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

            <ReactStars
            count={5}
            onChange={changeRating}
            size={24}
            color1={'#4e4e4e'}
            color2={'#a7a7a7'} 
            value={nowPlayingData.rating}/>

            <div className="volContainer">
                <div onClick={()=>{
                    if(volume.isMuted){
                        playerObject.playerClass.changeVolume(volume.previousVolume);
                        setVolume({...volume, isMuted:false, value:volume.previousVolume});
                    }
                    else{
                        playerObject.playerClass.changeVolume(0);
                        setVolume({...volume, isMuted:true, value:0});
                    }
                }}>
                    {volume.isMuted ? <VolumeX size='20'/> : <Volume2 size='20'/>}
                </div>
                <input value={volume.value} className="vs" type="range" min="0" max="1" step="any" onChange={(e)=>{playerObject.playerClass.changeVolume(e.target.value); setVolume({...volume, previousVolume:e.target.value, value:e.target.value})}}/>
            </div>
        </div>
    );
}

export default Controls;