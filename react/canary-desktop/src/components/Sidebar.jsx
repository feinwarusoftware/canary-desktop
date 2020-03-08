import React from 'react';
import { Disc, List, Settings } from 'react-feather';
import {
    BrowserRouter as Router,
    Route,
    Link,
    Switch
  } from "react-router-dom";
import '../styles/Sidebar.css'

function Sidebar(props){
    return(
        <div className="sidebar">
             <Link to="/usermusic"> <Disc size='40'/></Link>
             <br></br>
             <Link to="/queue"><List size='40'/></Link>
             <br></br>
             <Link to="/settings"><Settings size='40'/></Link>
        </div>
    );
}

export default Sidebar;