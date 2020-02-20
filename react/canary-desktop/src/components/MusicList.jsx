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

    if(data){
        console.log("tem coisas, vamos desenhar");
    }

    useEffect(()=>{
        if(!data){
            console.log("Dados em branco, carregando...");
            libObject.libClass.loadLib().then((data)=>{
                //console.log(data);
                setData(data);
            });
        }
    }, [data]);

    return (
      <div className="music-list">
          oi
      </div>
    );
  }
  
  export default MusicList;