async function withTimeout(promise, timeout) {
    return Promise.race([
        promise,
        new Promise((_, reject) =>
        setTimeout(() => reject(new Error('Operation timed out')), timeout)
        ),
    ]);
}


function fix_dev_name(dev_name){
    if(dev_name.includes(" ")){
        dev_name = dev_name.replaceAll(" ", "_");
    }
    return dev_name;
}

function unfix_dev_name(dev_name){
    if(dev_name.includes("_")){
        dev_name = dev_name.replaceAll("_", " ");
    }
    return dev_name;
}