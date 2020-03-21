import React from 'react';

function Rating({value, setRating}){    
    function convertRate(val){
        if(val % 1 && val % 0.5){
            let floored = Math.floor(val);
            let resto = (val - floored).toFixed(2);
            if(resto > 0.5){
                return Math.round(val);
            }
            return floored;
        }
        return val;
    }

    let v = convertRate(value / 2);

    function rate(e){
        console.log('Valor atual: ' + value);
        console.log('Valor novo: ' + e.target.value);

        setRating(e.target.value * 2);
    }

    return(
    <fieldset className="rating" onChange={rate}>
        <input type="radio" id="star5" name="rating" value="5" checked={v === 5}/><label for="star5"></label>
        <input type="radio" id="star4half" name="rating" value="4.5" checked={v === 4.5}/><label className="half" for="star4half"></label>
        <input type="radio" id="star4" name="rating" value="4"checked={v === 4}/><label className = "full" for="star4"></label>
        <input type="radio" id="star3half" name="rating" value="3.5" checked={v === 3.5}/><label className="half" for="star3half"></label>
        <input type="radio" id="star3" name="rating" value="3" checked={v === 3}/><label className = "full" for="star3"></label>
        <input type="radio" id="star2half" name="rating" value="2.5" checked={v === 2.5}/><label className="half" for="star2half"></label>
        <input type="radio" id="star2" name="rating" value="2" checked={v === 2}/><label className = "full" for="star2"></label>
        <input type="radio" id="star1half" name="rating" value="1.5" checked={v === 1.5}/><label className="half" for="star1half"></label>
        <input type="radio" id="star1" name="rating" value="1" checked={v === 1}/><label className = "full" for="star1"></label>
        <input type="radio" id="starhalf" name="rating" value="0.5" checked={v === 0.5}/><label className="half" for="starhalf"></label>
    </fieldset>
    );
}

export default Rating;