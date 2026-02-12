const submitBtn=document.getElementById('submitSharesBtn');
const inputs=[1,2,3,4,5].map(id=>({
    el:document.getElementById(`minister${id}`),
    id:id
}));

submitBtn.addEventListener('click',async()=>{
    const sessionId=localStorage.getItem('crtSessionId');
    if(!sessionId){
        alert('No active session found! Go back to admin panel.');
        return;
    }

    const shares=inputs
        .filter(item=>item.el.value.trim()!=='')
        .map(item=>({
            id:item.id,
            value:item.el.value
        }));

    if(shares.length<3){
        alert('Need at least 3 shares for reconstruction!');
        return;
    }

    try{
        submitBtn.disabled=true;
        submitBtn.textContent='Verifying...';

        const response=await fetch('/api/reconstruct',{
            method:'POST',
            headers:{'Content-Type':'application/json'},
            body:JSON.stringify({sessionId:sessionId,shares:shares})
        });

        const result=await response.json();
        const submittedIds=shares.map(s=>s.id).join(',');

        if(result.success){
            alert('LAUNCH AUTHORIZED! Secret: '+result.secret);
            window.location.href=`admin.html?status=success&shares=${submittedIds}`;
        }else{
            alert('LAUNCH ABORTED! Shares verification failed.');
            window.location.href=`admin.html?status=failed&shares=${submittedIds}`;
        }
    }catch(e){
        console.error(e);
        alert('System Error: '+e.message);
    }finally{
        submitBtn.disabled=false;
        submitBtn.textContent='Submit All Shares';
    }
});

document.getElementById('abortLaunchBtn').addEventListener('click',async()=>{
    if(confirm('Are you sure you want to ABORT the launch sequence?')){
        const sessionId=localStorage.getItem('crtSessionId');
        if(sessionId){
            await fetch('/api/abort',{
                method:'POST',
                headers:{'Content-Type':'application/json'},
                body:JSON.stringify({sessionId:sessionId})
            });
            localStorage.removeItem('crtSessionId');
        }
        window.location.href='admin.html?status=failed';
    }
});
