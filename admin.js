const generateBtn=document.getElementById('generateSharesBtn');
const launchBtn=document.getElementById('initiateLaunchBtn');
const abortBtn=document.getElementById('abortSessionBtn');
const progressBar=document.getElementById('generationProgress');
const timerDisplay=document.getElementById('timer');
const launchMessage=document.getElementById('launch-message');

let countdown;
abortBtn.disabled=true; 
launchBtn.disabled=true; 

window.onload=()=>{
    abortBtn.disabled=true;
    launchBtn.disabled=true;
    const urlParams=new URLSearchParams(window.location.search);
    const status=urlParams.get('status');
    if(status==='success'){
        launchMessage.innerHTML='<span style="color: green; font-weight: bold; font-size: 1.5em;">LAUNCH SEQUENCE INITIATED: AUTHORIZED</span>';
        localStorage.removeItem('crtSessionId'); 
    }else if(status==='failed'){
        launchMessage.innerHTML='<span style="color: red; font-weight: bold; font-size: 1.5em;">LAUNCH SEQUENCE ABORTED: UNAUTHORIZED</span>';
        localStorage.removeItem('crtSessionId');
    }
};

generateBtn.addEventListener('click',async()=>{
    try {
        generateBtn.disabled=true;
        launchMessage.textContent='Generating...';

        progressBar.style.width = '50%';
        const response=await fetch('/api/generate',{method:'POST'});
        const data=await response.json();
        if (data.error) throw new Error(data.error);

        localStorage.setItem('crtSessionId', data.sessionId);
        progressBar.style.width='100%';
        data.shares.forEach(s=>{
            const box=document.getElementById(`minister${s.id}`);
            if(box){
                box.innerHTML=`<strong>Minister ${s.id}</strong><br><small>Ready for manual entry</small>`;
                box.classList.add('generated');
            }
        });

        launchMessage.textContent='Shares Generated. Ready to Initiate Launch.';
        abortBtn.disabled = false;
        launchBtn.disabled = false;
        startTimer();

    }catch(e){
        console.error(e);
        launchMessage.textContent='Error generating shares: '+e.message;
        launchMessage.style.color='red';
        generateBtn.disabled=false;
    }
});

abortBtn.addEventListener('click',async()=>{
    const sessionId=localStorage.getItem('crtSessionId');
    if (sessionId){
        await fetch('/api/abort',{
            method:'POST',
            headers:{ 'Content-Type': 'application/json' },
            body: JSON.stringify({ sessionId })
        });
        localStorage.removeItem('crtSessionId');
    }
    location.reload();
});

launchBtn.addEventListener('click',()=>{
    window.location.href='minister.html';
});

function startTimer(){
    let timeLeft=300; 
    clearInterval(countdown);
    countdown=setInterval(()=>{
        const m=Math.floor(timeLeft/60);
        const s=timeLeft%60;
        timerDisplay.textContent=`Session Expires in: ${m}:${s<10 ?'0':''}${s}`;
        if(timeLeft<=0) {
            clearInterval(countdown);
            localStorage.removeItem('crtSessionId');
            alert('Session Expired!');
            location.reload();
        }
        timeLeft--;
    },1000);
}
