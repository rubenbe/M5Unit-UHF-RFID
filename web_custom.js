(function() {
    var style = 'position:fixed;z-index:9999;padding:10px 20px;color:white;text-decoration:none;border-radius:8px;font-family:Roboto,sans-serif;font-size:14px;box-shadow:0 2px 8px rgba(0,0,0,0.3);';
    var a1 = document.createElement('a');
    a1.href = '/tags.csv';
    a1.textContent = 'Tags CSV';
    a1.style.cssText = style + 'bottom:16px;right:16px;background:#03a9f4;';
    document.body.appendChild(a1);
    var a2 = document.createElement('a');
    a2.href = '/runners.csv';
    a2.textContent = 'Runners CSV';
    a2.style.cssText = style + 'bottom:56px;right:16px;background:#4caf50;';
    document.body.appendChild(a2);
})();
