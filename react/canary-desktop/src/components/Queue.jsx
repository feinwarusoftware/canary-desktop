import React, { useState, useEffect } from 'react';

function Queue({playerObject}) {
    const [queue, setQueue] = useState(false);
    const [isUpdate, setIsUpdate] = useState(false);

    function printQueue(data) {
        const qS = data.map((song, index) =>
            <li key={index}>{song.title} {song.isPlayingNow ? "Now Playing" : ""}</li>
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