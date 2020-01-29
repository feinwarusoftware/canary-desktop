import React from 'react';
import { useParams } from "react-router";

function AlbumPage(props){
    const params = useParams();

    return(
        <div>
            oi eu sou a album page do {params.id}
        </div>
    );
}

export default AlbumPage;