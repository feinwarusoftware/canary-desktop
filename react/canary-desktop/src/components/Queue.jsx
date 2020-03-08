import React, { useState, useEffect } from 'react';
import Song from './Song';

function Queue({playerObject}) {
    //MAYBE TODO: optimize this based on the AlbumPage logic (default values based on properties)
    const [queue, setQueue] = useState(false);
    const [isUpdate, setIsUpdate] = useState(false);

    function printQueue(data) {
        const qS = data.map((song, index) =>
            <Song key={index} name={song.title[0]} artist={song.artist[0]} album={song.album[0]} length={song.lengthInSeconds} songCallback={()=> playerObject.playerClass.jumpTo(index)} nowPlaying={song.isPlayingNow ? true : false}>
            </Song>
        );

        setQueue(qS);
    }

    function updateData(data) {
        if (!isUpdate) {
            setIsUpdate(true);
        }

        printQueue(data);
    }

    /*eslint-disable */
    useEffect(() => {
        playerObject.queueUpdate.connect(updateData);

        if (!isUpdate) {
            playerObject.playerClass.getQueue().then((data) => {
                printQueue(data);
            });
        }

        return () => {
            playerObject.queueUpdate.disconnect(updateData);
        }
    }, [isUpdate]);
    /*eslint-enable */

    return (
        <div>
            {queue}
        </div>
    );
}

export default Queue;