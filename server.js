const express=require('express');
const bodyParser=require('body-parser');
const { exec }=require('child_process');
const path=require('path');

const app=express();
const PORT=3000;

app.use(bodyParser.json());
app.use(express.static(__dirname));

const sessions={};

function runNLC(mode,inputData=''){
    return new Promise((resolve,reject) =>{
        const command=`NLC.exe ${mode}`;
        const child=exec(command, (error, stdout, stderr) =>{
            if(error){
                console.error(`Exec Error: ${error.message}`);
                reject(error);
                return;
            }
            try{
                const jsonStart=stdout.indexOf('{');
                const jsonEnd=stdout.lastIndexOf('}');
                if (jsonStart!==-1 && jsonEnd!==-1){
                    const jsonStr=stdout.substring(jsonStart, jsonEnd + 1);
                    resolve(JSON.parse(jsonStr));
                }else{
                    reject(new Error("Invalid output from C++ backend: " + stdout));
                }
            }catch(e){
                reject(e);
            }
        });

        if (inputData){
            child.stdin.write(inputData);
            child.stdin.end();
        }
    });
}

setInterval(()=>{
    const now=Date.now();
    for(const id in sessions){
        if(now - sessions[id].createdAt>10*60*1000){
            delete sessions[id];
            console.log(`Session ${id} cleaned up.`);
        }
    }
},60 *1000);

app.post('/api/generate',async(req, res)=>{
    try{
        const result=await runNLC('generate');
        const sessionId=Date.now().toString();
        sessions[sessionId]={
            secret: result.secret,
            allShares: result.shares,
            createdAt: Date.now()
        };
        console.log(`\n=== SESSION ${sessionId} GENERATED ===`);
        console.log(`SECRET: ${result.secret}`);
        console.log("SHARES DISTRIBUTED:");
        result.shares.forEach(s => {
            console.log(`Minister ${s.id}: Share=${s.share}, Mod=${s.mod}`);
        });
        console.log("======================================\n");
        const publicShares=result.shares.map(s =>({
            id: s.id,
            mod: s.mod,
        }));

        res.json({
            sessionId: sessionId,
            shares: publicShares
        });
    }catch(error){
        console.error(error);
        res.status(500).json({ error: error.message });
    }
});

app.post('/api/reconstruct',async (req,res)=>{
    const {sessionId,shares}=req.body;
    if(!sessions[sessionId]){
        return res.status(404).json({ error: "Session expired or invalid" });
    }

    const session=sessions[sessionId];

    if(!shares||!Array.isArray(shares)||shares.length===0){
        return res.status(400).json({ error: "No shares provided" });
    }

    try{
        const inputShares=shares.map(s =>{
            const original=session.allShares.find(os => os.id==s.id);
            if (!original) throw new Error(`Invalid Minister ID ${s.id}`);
            return {
                id: original.id,
                share: String(s.value),
                mod: String(original.mod)
            };
        });

        const inputJson=JSON.stringify({ shares: inputShares });

        const result=await runNLC('reconstruct', inputJson);

        if (result.secret && result.secret==session.secret) {
            delete sessions[sessionId];
            console.log(`Session ${sessionId} verified and deleted from memory.`);
            res.json({ success: true, secret: result.secret });
        } else {
            res.json({ success: false, message: "Reconstruction mismatch or failure" });
        }
    }catch(e){
        console.error(e);
        res.status(500).json({error: e.message});
    }
});

app.post('/api/abort',(req, res)=>{
    const { sessionId }=req.body;
    if(sessions[sessionId]){
        delete sessions[sessionId];
    }
    res.json({success: true});
});

app.listen(PORT,() =>{
    console.log(`Server listening on port ${PORT}`);
});
