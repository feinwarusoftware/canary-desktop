import React from 'react';
import '../styles/MusicList.css'

function MusicList() {
    var arr = [];
    for(var i=1;i<339;i++){
        arr.push(i);
    }

    const list = arr.map((index) =>
        <div className="listItem" key={index} style={{
            backgroundImage:`url(./cache/covers/${index}.png)`
        }}></div>
    );

    return (
      <div className="music-list">
          {list}
      </div>
    );
  }
  
  export default MusicList;