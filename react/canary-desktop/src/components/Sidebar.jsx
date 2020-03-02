import React from 'react';
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
             <Link to="/usermusic">To the music list</Link>
             <br></br>
             <Link to="/queue">To the queue page</Link>
        </div>
    );
}

export default Sidebar;