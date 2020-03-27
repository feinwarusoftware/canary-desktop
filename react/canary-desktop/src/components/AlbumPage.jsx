import React, { useState, useEffect } from 'react';
import { useParams } from "react-router";
//import { Menu, Item, Separator, Submenu, MenuProvider } from 'react-contexify';
import Song from './Song';
import 'react-contexify/dist/ReactContexify.min.css';
import '../styles/AlbumPage.css'

function AlbumPage({location, playerObject}){
    //TODO: when song update, check if it still belongs to the album
    const [nowPlayingSong, setNowPlayingSong] = useState(playerObject.nowPlaying.dir);
    const params = useParams();
    const data = location.state.data;

    function c({dir}){ //TODO: remove this function XD
        setNowPlayingSong(dir);
    }

    /*function changeSong(pos, songObject){
        if(songObject.albumid !== data.tracks[0].albumid){
            return;
        }

        for(let i=0; i < data.tracks.length; i++){
            if(data.tracks[i].dir === songObject.dir){
                data.tracks[i] = songObject;
                break;
            }
        }
    }*/

    useEffect(() => {
        playerObject.setNowPlayingInfo.connect(c);

        return() =>{
            playerObject.setNowPlayingInfo.disconnect(c);
        }
    });
    let albumLength = 0;
    console.log(data);

    const songCall = (i, albumid, tracks) => {
        console.log('mesmo álbum')
        if(albumid === playerObject.nowPlaying.albumid){
            return playerObject.playerClass.jumpTo(i);
        }
        playerObject.insertAlbumToQueue(tracks, i);
    }

    const tracks = data.tracks.map((song, index) =>{
        albumLength = albumLength + song.lengthInSeconds;

        return(<Song key={index} number={index + 1} name={song.title} length={song.lengthInSeconds} songCallback={()=> {songCall(index, song.albumid, data.tracks);}} 
        //no need to compare albums cuz there are not two songs with the same dir
        nowPlaying={nowPlayingSong === song.dir ? true : false}>
        </Song>   
        );
    });

    return(
        <div className="albumPage">
            <div className="album-info">
                <div className="coverArt" style={{backgroundImage:`url('${process.env.PUBLIC_URL}/cache/covers/${params.id}.jpg')`}}></div>
                <div className="album-databox">
                    <span className="album-databox-name">{data.name}</span>
                    <span>by {data.artist}</span>
                    <span>{data.date ? `${new Date(data.date).getFullYear()} - ` : ""}{data.tracks.length} songs - {Math.floor(albumLength / 60)}min</span>
                    <button onClick={()=>
                        playerObject.playerClass.getShuffle((shuffle)=>{
                            let s;
                            if(shuffle){
                                s = Math.floor(Math.random() * (data.tracks.length + 1));
                            }
                            else{
                                s = 0;
                            }

                            songCall(s, data.tracks[0].albumid, data.tracks);
                        })
                    }>Play</button>
                </div>
            </div>
            <div className="tracklist">
                {tracks}
            </div>
        </div>
    );
}

export default AlbumPage;