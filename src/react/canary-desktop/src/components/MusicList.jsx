import React, { useState } from 'react';
import {
    BrowserRouter as Router,
    Link
  } from "react-router-dom";
import '../styles/MusicList.css';
import LazyLoad from 'react-lazyload';

function MusicList(props){
    const data = JSON.parse(props.data);
    const albums = Object.keys(data);

    //const listItems = <div>oi</div>

    const listItems = albums.map((key) =>
            <div key={key} className="albumContainer">
                <img src={process.env.PUBLIC_URL + '/cache/covers/' + key + '.png'} onError={(e)=>{e.target.onerror = null; e.target.src= process.env.PUBLIC_URL + '/cache/covers/notfound.png'}} />
                <Link to={{
                    pathname:`/album/${key}`,
                    state:{
                        data:data[key]
                    }
                }}><span className="album">{data[key].name}</span></Link>
                <span className="artist">{data[key].artist}</span>
            </div>
    );

    return(
        <div>
           {listItems}
        </div>
    );
}

export default MusicList;

//MAYBE TODO: use props on App istead of states here (using playerObject as a prop)? I think it's more organized the current way, so I'll keep for now. but maybe change.