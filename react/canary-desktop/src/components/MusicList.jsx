import React, { useState, useEffect } from 'react';
import '../styles/MusicList.css'

function MusicList({libObject}) {
    const [data, setData] = useState(false);
    /*var arr = [];
    for(var i=1;i<339;i++){
        arr.push(i);
    }

    const list = arr.map((index) =>
        <div className="listItem" key={index} style={{
            backgroundImage:`url(./cache/covers/${index}.png)`
        }}></div>
    );*/

            /*let loadedAlbums = [];
        let albums = {};

        data.forEach(function(song){
            if(loadedAlbums.includes(song.albumid)){
                albums[song.albumid].push(song);
                return; 
            }

            loadedAlbums.push(song.albumid);
            albums[song.albumid] = [];
            albums[song.albumid].push(song);
        });*/

    /*if(data){
        console.log("tem coisas, vamos desenhar");
        console.log(data);
    }*/

    useEffect(()=>{
        if(!data){
            console.log("Dados em branco, carregando...");
            libObject.libClass.loadLib().then((res)=>{
                let loadedAlbums = [];
                let albums = {};
        
                res.forEach(function(song){
                    if(loadedAlbums.includes(song.albumid)){
                        albums[song.albumid].tracks.push(song);
                        return; 
                    }
        
                    loadedAlbums.push(song.albumid);
                    albums[song.albumid] = {
                        name:"",
                        artist:"",
                        date:""
                    };
        
                    if(song.album){
                        albums[song.albumid]["name"] = song.album[0]
                    }
                    if(song.artist){
                        albums[song.albumid]["artist"] = song.artist[0]
                    }
                    if(song.date){
                        albums[song.albumid]["date"] = song.date[0]
                    }
        
                    albums[song.albumid]["tracks"] = [];
                    albums[song.albumid]["tracks"].push(song);
                });

                const a = Object.keys(albums).map(key => 
                    <div key={key}>
                        <span>{albums[key].artist} - {albums[key].name}</span>
                        <img src={process.env.PUBLIC_URL + `/cache/covers/${key}.jpg`} />
                    </div>
                );

                setData(a);
            });
        }
    }, [data]);

    return (
      <div className="music-list albumlist">
          {data}
      </div>
    );
  }
  
  export default MusicList;