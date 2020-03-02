import React, { useState, useEffect } from 'react';
import { useParams } from "react-router";
import { Menu, Item, Separator, Submenu, MenuProvider } from 'react-contexify';
import Song from './Song';
import 'react-contexify/dist/ReactContexify.min.css';
import '../styles/AlbumPage.css'

function AlbumPage({location, playerObject}){
    const [nowPlayingSong, setNowPlayingSong] = useState(playerObject.nowPlaying.dir);

    function c({dir}){ //TODO: remove this function XD
        setNowPlayingSong(dir);
    }

    useEffect(() => {
        playerObject.setNowPlayingInfo.connect(c);

        return() =>{
            playerObject.setNowPlayingInfo.disconnect(c);
        }
    },  []);

    const params = useParams();
    const data = location.state.data;
    console.log(data);

    const songCall = (i, albumid, tracks) => {
        console.log('mesmo álbum')
        if(albumid === playerObject.nowPlaying.albumid){
            return playerObject.playerClass.jumpTo(i);
        }
        playerObject.insertAlbumToQueue(tracks, i);
    }

    const tracks = data.tracks.map((song, index) =>
        <Song number={index + 1} name={song.title[0]} length={song.lengthInSeconds} songCallback={()=> songCall(index, song.albumid, data.tracks)} 
        //no need to compare albums cuz there are not two songs with the same dir
        nowPlaying={nowPlayingSong === song.dir ? true : false}>
        </Song>
    );

    return(
        <div className="albumPage">
            <img src={process.env.PUBLIC_URL + `/cache/covers/${params.id}.jpg`} />
            <br></br>
            {data.name}
            <br></br>
            by {data.artist}
            <br></br>
            {tracks}
        </div>
    );
}

export default AlbumPage;