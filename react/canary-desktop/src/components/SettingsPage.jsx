import React from 'react';

function SettingsPage({lastfmObject}){

    return(<div>
        <b>Last.fm</b>
        <br></br>
        <button onClick={()=>{
            lastfmObject.getAuthFromUser();
        }}>Sync</button>
    </div>);
}

export default SettingsPage;