import { useState, useEffect } from 'react'
import Head from 'next/head'

const API_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8080'

const EMOTIONS = [
  { label: 'Happy', icon: '😊', color: '#FFD93D', id: 0 },
  { label: 'Sad', icon: '😢', color: '#6C5CE7', id: 1 },
  { label: 'Angry', icon: '😠', color: '#E74C3C', id: 2 },
  { label: 'Surprised', icon: '😮', color: '#FD79A8', id: 3 },
  { label: 'Neutral', icon: '😐', color: '#74B9FF', id: 4 },
]

const MODELS = [
  { value: 'emotion-v1', label: 'Emotion V1', desc: 'Fast · Mock Detection', badge: 'FAST' },
  { value: 'emotion-v2', label: 'Emotion V2', desc: 'Advanced · High Accuracy', badge: 'PRO' },
  { value: 'mock', label: 'Mock Detector', desc: 'Testing · Always Happy', badge: 'TEST' },
  { value: 'advanced', label: 'Advanced Engine', desc: 'ML-Powered · Variable Output', badge: 'ML' },
]

function simulateDemoResponse(modelId, imageData) {
  const hash = imageData.split('').reduce((acc, c) => acc + c.charCodeAt(0), 0)
  const emotion = EMOTIONS[hash % EMOTIONS.length]
  const confidence = modelId === 'emotion-v1' || modelId === 'mock'
    ? 0.94
    : 0.75 + (hash % 24) / 100
  return {
    classLabel: emotion.label,
    classId: emotion.id,
    confidence: parseFloat(confidence.toFixed(4)),
    inferenceTimeMs: modelId === 'emotion-v1' || modelId === 'mock' ? 42 : 20 + (hash % 80),
    timestamp: new Date().toISOString().slice(0, 19),
    source: 'demo'
  }
}

export default function Home() {
  const [modelId, setModelId] = useState('emotion-v1')
  const [imageData, setImageData] = useState('test-image-data')
  const [result, setResult] = useState(null)
  const [loading, setLoading] = useState(false)
  const [serverStatus, setServerStatus] = useState('checking')
  const [activeTab, setActiveTab] = useState('demo')
  const [animateResult, setAnimateResult] = useState(false)

  useEffect(() => { checkServer() }, [])

  const checkServer = async () => {
    try {
      const controller = new AbortController()
      const timeout = setTimeout(() => controller.abort(), 5000)
      const res = await fetch(`${API_URL}/health`, { signal: controller.signal })
      clearTimeout(timeout)
      setServerStatus(res.ok ? 'online' : 'offline')
    } catch { setServerStatus('offline') }
  }

  const handleDetect = async (e) => {
    e.preventDefault()
    setLoading(true)
    setResult(null)
    setAnimateResult(false)

    if (activeTab === 'demo' || serverStatus !== 'online') {
      await new Promise(r => setTimeout(r, 1000 + Math.random() * 800))
      setResult({ ok: true, data: simulateDemoResponse(modelId, imageData) })
    } else {
      try {
        const res = await fetch(`${API_URL}/api/v1/emotion`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ modelId, imageBase64: imageData })
        })
        const data = await res.json()
        setResult({ ok: res.ok, data })
      } catch {
        setResult({ ok: true, data: { ...simulateDemoResponse(modelId, imageData), source: 'demo-fallback' } })
      }
    }
    setLoading(false)
    setTimeout(() => setAnimateResult(true), 50)
  }

  const currentEmotion = result && result.ok ? EMOTIONS.find(e => e.label === result.data.classLabel) : null

  return (
    <>
      <Head>
        <title>Spectra Vision - AI Emotion Detection</title>
        <meta name="description" content="Production C++ emotion detection API with Strategy, Factory, Singleton and DI patterns" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
      </Head>
      <div className="page">
        <div className="orb orb1" /><div className="orb orb2" /><div className="orb orb3" />
        <header className="hdr">
          <div className="hdr-inner">
            <div className="brand">
              <span className="brand-icon">🎭</span>
              <div>
                <div className="brand-name">Spectra Vision</div>
                <div className="brand-sub">AI Emotion Detection Engine</div>
              </div>
            </div>
            <div className="hdr-right">
              <span className="chip chip-cpp">C++17</span>
              <span className="chip chip-pat">4 Design Patterns</span>
              <span className={`pill pill-${serverStatus}`}>
                <span className="pdot" />
                {serverStatus === 'online' ? 'API Live' : serverStatus === 'checking' ? 'Checking...' : 'Demo Mode'}
              </span>
            </div>
          </div>
        </header>

        <main className="main">
          <div className="hero">
            <h1 className="hero-h1">Real-time Emotion Detection<span className="hero-grad"> Powered by C++</span></h1>
            <p className="hero-p">Production-grade backend with Strategy, Factory, Singleton &amp; Dependency Injection patterns. Sub-100ms inference.</p>
          </div>

          <div className="tabs">
            <button className={`tab ${activeTab==='demo'?'tab-on':''}`} onClick={() => setActiveTab('demo')}>⚡ Demo Mode</button>
            <button className={`tab ${activeTab==='live'?'tab-on':''}`} onClick={() => setActiveTab('live')}>
              🌐 Live API{serverStatus !== 'online' && <span className="tab-off"> · offline</span>}
            </button>
          </div>

          <div className="grid2">
            <div className="panel">
              <div className="panel-title">Detect Emotion</div>
              <form onSubmit={handleDetect}>
                <div className="field">
                  <div className="flabel">Detection Model</div>
                  <div className="mgrid">
                    {MODELS.map(m => (
                      <button key={m.value} type="button" className={`mcard ${modelId===m.value?'mcard-on':''}`} onClick={() => setModelId(m.value)}>
                        <span className="mbadge">{m.badge}</span>
                        <span className="mname">{m.label}</span>
                        <span className="mdesc">{m.desc}</span>
                      </button>
                    ))}
                  </div>
                </div>
                <div className="field">
                  <label className="flabel" htmlFor="img">Image Input</label>
                  <textarea id="img" className="tarea" value={imageData} onChange={e => setImageData(e.target.value)} placeholder="Enter base64 image data or any text..." rows={4} />
                  <div className="fhint">{activeTab==='demo' ? '⚡ Demo mode — no backend needed' : serverStatus==='online' ? '🌐 Connected to live C++ API' : '⚡ Backend offline — using demo mode'}</div>
                </div>
                <button type="submit" className="dbtn" disabled={loading}>
                  {loading ? <><span className="spin" /> Analyzing...</> : <>✨ Detect Emotion</>}
                </button>
              </form>
            </div>

            <div className="panel rpanel">
              <div className="panel-title">Result</div>
              {!result && !loading && <div className="empty"><div className="empty-ico">🎭</div><p>Run detection to see results</p></div>}
              {loading && <div className="empty"><div className="lring" /><p>Processing with C++ engine...</p><span className="lsub">Running inference pipeline</span></div>}
              {result && !loading && (
                <div className={`res ${animateResult?'res-in':''}`}>
                  {result.ok && currentEmotion ? (
                    <>
                      <div className="ehero" style={{'--ec': currentEmotion.color}}>
                        <div className="eico">{currentEmotion.icon}</div>
                        <div className="elabel">{currentEmotion.label}</div>
                        <div className="cbar-wrap">
                          <div className="cbar-track"><div className="cbar-fill" style={{width: animateResult ? `${result.data.confidence*100}%` : '0%', background: currentEmotion.color}} /></div>
                          <span className="cpct">{(result.data.confidence*100).toFixed(1)}%</span>
                        </div>
                      </div>
                      <div className="mgrid2">
                        <div className="mbox"><span className="mkey">Inference Time</span><span className="mval">{result.data.inferenceTimeMs}ms</span></div>
                        <div className="mbox"><span className="mkey">Class ID</span><span className="mval">#{result.data.classId}</span></div>
                        <div className="mbox"><span className="mkey">Model</span><span className="mval">{modelId}</span></div>
                        <div className="mbox"><span className="mkey">Mode</span><span className="mval">{result.data.source === 'demo' || result.data.source === 'demo-fallback' ? 'Demo' : 'Live'}</span></div>
                      </div>
                      <div className="jblock">
                        <div className="jhdr"><span>JSON Response</span><span className="jok">200 OK</span></div>
                        <pre className="jbody">{JSON.stringify(result.data, null, 2)}</pre>
                      </div>
                    </>
                  ) : <div className="empty"><div>⚠️</div><p>Detection failed. Try demo mode.</p></div>}
                </div>
              )}
            </div>
          </div>

          <div className="arch-title">C++ Architecture Highlights</div>
          <div className="agrid">
            {[
              {ico:'🔄', title:'Strategy Pattern', desc:'Swappable detection algorithms at runtime — MockDetector & AdvancedDetector share a common interface', code:'EmotionDetectionStrategy'},
              {ico:'🏭', title:'Factory Pattern', desc:'EmotionModelFactory creates the right detector from modelId without tight coupling to the client', code:'EmotionModelFactory::create()'},
              {ico:'♾️', title:'Singleton Pattern', desc:"Thread-safe Meyer's Singleton logger — one global instance across all translation units", code:'Logger::getInstance()'},
              {ico:'💉', title:'Dependency Injection', desc:'APIHandler receives detector via factory — decoupled, testable and extensible by design', code:'APIHandler(detector)'},
            ].map(c => (
              <div key={c.title} className="acard">
                <div className="aico">{c.ico}</div>
                <h4 className="atitle">{c.title}</h4>
                <p className="adesc">{c.desc}</p>
                <code className="acode">{c.code}</code>
              </div>
            ))}
          </div>

          <div className="sstack">
            {['C++17','CMake 3.20','cpp-httplib','nlohmann/json','Docker','GitHub Actions','Next.js 14','Vercel'].map(t => <span key={t} className="spill">{t}</span>)}
          </div>
        </main>

        <footer className="footer">
          <p>Built to showcase C++ design patterns &amp; modern full-stack architecture</p>
          <a href="https://github.com/gavinspet/spectra-vision" target="_blank" rel="noopener noreferrer" className="ghlink">View Source on GitHub →</a>
        </footer>
      </div>

      <style jsx global>{`
        *,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
        html{scroll-behavior:smooth}
        body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:#080818;color:#e0e0ff;min-height:100vh}
        .page{position:relative;min-height:100vh;overflow-x:hidden}
        .orb{position:fixed;border-radius:50%;filter:blur(80px);opacity:.1;pointer-events:none;z-index:0;animation:float 20s ease-in-out infinite}
        .orb1{width:600px;height:600px;background:#667eea;top:-200px;left:-200px}
        .orb2{width:500px;height:500px;background:#f093fb;bottom:-200px;right:-100px;animation-delay:-7s}
        .orb3{width:400px;height:400px;background:#4facfe;top:40%;left:40%;animation-delay:-14s}
        @keyframes float{0%,100%{transform:translate(0,0)}33%{transform:translate(30px,-30px)}66%{transform:translate(-20px,20px)}}
        .hdr{position:sticky;top:0;z-index:100;background:rgba(8,8,24,.85);backdrop-filter:blur(20px);border-bottom:1px solid rgba(255,255,255,.06)}
        .hdr-inner{max-width:1100px;margin:0 auto;padding:14px 24px;display:flex;align-items:center;justify-content:space-between;gap:12px;flex-wrap:wrap}
        .brand{display:flex;align-items:center;gap:12px}
        .brand-icon{font-size:34px}
        .brand-name{font-size:1.3em;font-weight:800;background:linear-gradient(135deg,#a78bfa,#60a5fa);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text}
        .brand-sub{font-size:.75em;color:rgba(255,255,255,.35);margin-top:2px}
        .hdr-right{display:flex;align-items:center;gap:8px;flex-wrap:wrap}
        .chip{padding:5px 11px;border-radius:20px;font-size:.72em;font-weight:700}
        .chip-cpp{background:rgba(102,126,234,.2);color:#a78bfa;border:1px solid rgba(167,139,250,.3)}
        .chip-pat{background:rgba(240,147,251,.12);color:#f0abfc;border:1px solid rgba(240,171,252,.3)}
        .pill{display:flex;align-items:center;gap:6px;padding:5px 13px;border-radius:20px;font-size:.78em;font-weight:600;border:1px solid rgba(255,255,255,.1);background:rgba(255,255,255,.05)}
        .pill-online{color:#4ade80;border-color:rgba(74,222,128,.3);background:rgba(74,222,128,.08)}
        .pill-offline,.pill-checking{color:#94a3b8}
        .pdot{width:7px;height:7px;border-radius:50%;background:currentColor;animation:pulse 2s infinite}
        @keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}
        .main{position:relative;z-index:1;max-width:1100px;margin:0 auto;padding:60px 24px 40px}
        .hero{text-align:center;margin-bottom:48px}
        .hero-h1{font-size:clamp(1.8em,4vw,2.9em);font-weight:800;line-height:1.2;margin-bottom:14px;color:#fff}
        .hero-grad{display:block;background:linear-gradient(135deg,#a78bfa 0%,#60a5fa 50%,#f0abfc 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text}
        .hero-p{max-width:600px;margin:0 auto;color:rgba(255,255,255,.45);font-size:1.05em;line-height:1.7}
        .tabs{display:flex;gap:6px;margin-bottom:28px;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.08);border-radius:12px;padding:5px;width:fit-content}
        .tab{padding:9px 20px;border-radius:8px;font-size:.88em;font-weight:600;border:none;cursor:pointer;background:transparent;color:rgba(255,255,255,.45);transition:all .2s;display:flex;align-items:center;gap:5px;width:auto;transform:none}
        .tab:hover{color:#fff;background:rgba(255,255,255,.06);box-shadow:none;transform:none}
        .tab-on{background:linear-gradient(135deg,#667eea,#764ba2)!important;color:#fff!important;box-shadow:0 4px 15px rgba(102,126,234,.3)}
        .tab-off{font-size:.78em;color:rgba(255,255,255,.35);font-weight:400}
        .grid2{display:grid;grid-template-columns:1fr 1fr;gap:22px;margin-bottom:56px}
        @media(max-width:768px){.grid2{grid-template-columns:1fr}}
        .panel{background:rgba(18,18,38,.75);backdrop-filter:blur(20px);border:1px solid rgba(255,255,255,.08);border-radius:20px;padding:28px}
        .panel-title{font-size:1.05em;font-weight:700;color:rgba(255,255,255,.75);margin-bottom:22px;padding-bottom:14px;border-bottom:1px solid rgba(255,255,255,.07)}
        .field{margin-bottom:20px}
        .flabel{display:block;font-size:.8em;font-weight:600;color:rgba(255,255,255,.45);text-transform:uppercase;letter-spacing:.8px;margin-bottom:9px}
        .fhint{font-size:.75em;color:rgba(255,255,255,.28);margin-top:7px}
        .mgrid{display:grid;grid-template-columns:1fr 1fr;gap:7px}
        .mcard{padding:11px;background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.08);border-radius:10px;cursor:pointer;text-align:left;transition:all .2s;display:flex;flex-direction:column;gap:3px;width:100%;transform:none;box-shadow:none;color:#e0e0ff}
        .mcard:hover{border-color:rgba(167,139,250,.4);background:rgba(167,139,250,.06);transform:none;box-shadow:none}
        .mcard-on{border-color:#a78bfa!important;background:rgba(167,139,250,.12)!important;box-shadow:0 0 0 1px rgba(167,139,250,.2)!important}
        .mbadge{font-size:.62em;font-weight:800;color:#a78bfa;letter-spacing:.5px}
        .mname{font-size:.87em;font-weight:600;color:#fff}
        .mdesc{font-size:.72em;color:rgba(255,255,255,.38)}
        .tarea{width:100%;padding:13px;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.1);border-radius:10px;color:#e0e0ff;font-family:'Monaco','Courier New',monospace;font-size:.86em;resize:vertical;transition:border-color .2s}
        .tarea:focus{outline:none;border-color:#a78bfa;background:rgba(167,139,250,.05)}
        .tarea::placeholder{color:rgba(255,255,255,.18)}
        .dbtn{width:100%;padding:14px;background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;border:none;border-radius:12px;font-size:.97em;font-weight:700;cursor:pointer;transition:all .3s;display:flex;align-items:center;justify-content:center;gap:7px;margin-top:6px}
        .dbtn:hover:not(:disabled){transform:translateY(-2px);box-shadow:0 10px 28px rgba(102,126,234,.4)}
        .dbtn:disabled{opacity:.6;cursor:not-allowed;transform:none}
        .spin{display:inline-block;width:15px;height:15px;border:2px solid rgba(255,255,255,.3);border-top-color:#fff;border-radius:50%;animation:spin .8s linear infinite}
        @keyframes spin{to{transform:rotate(360deg)}}
        .rpanel{min-height:380px}
        .empty{display:flex;flex-direction:column;align-items:center;justify-content:center;height:280px;gap:11px;color:rgba(255,255,255,.28);text-align:center;font-size:.92em}
        .empty-ico{font-size:46px;opacity:.35}
        .lring{width:46px;height:46px;border:3px solid rgba(167,139,250,.2);border-top-color:#a78bfa;border-radius:50%;animation:spin 1s linear infinite}
        .lsub{font-size:.76em;color:rgba(255,255,255,.2)}
        .res{opacity:0;transform:translateY(10px);transition:all .4s ease}
        .res-in{opacity:1;transform:translateY(0)}
        .ehero{text-align:center;padding:26px 18px;background:rgba(255,255,255,.03);border-radius:14px;margin-bottom:14px;border:1px solid rgba(255,255,255,.07)}
        .eico{font-size:65px;margin-bottom:8px}
        .elabel{font-size:1.7em;font-weight:800;color:var(--ec);margin-bottom:14px}
        .cbar-wrap{display:flex;align-items:center;gap:9px}
        .cbar-track{flex:1;height:8px;background:rgba(255,255,255,.1);border-radius:10px;overflow:hidden}
        .cbar-fill{height:100%;border-radius:10px;transition:width .8s cubic-bezier(.4,0,.2,1)}
        .cpct{font-size:.88em;font-weight:700;color:#fff;min-width:42px}
        .mgrid2{display:grid;grid-template-columns:1fr 1fr;gap:7px;margin-bottom:14px}
        .mbox{background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.07);border-radius:10px;padding:11px;display:flex;flex-direction:column;gap:4px}
        .mkey{font-size:.7em;font-weight:600;color:rgba(255,255,255,.38);text-transform:uppercase;letter-spacing:.5px}
        .mval{font-size:.97em;font-weight:700;color:#fff;font-family:'Monaco',monospace}
        .jblock{border-radius:10px;overflow:hidden;border:1px solid rgba(255,255,255,.07)}
        .jhdr{display:flex;justify-content:space-between;align-items:center;padding:9px 13px;background:rgba(255,255,255,.05);font-size:.78em;color:rgba(255,255,255,.45);font-weight:600}
        .jok{color:#4ade80;font-weight:700}
        .jbody{background:rgba(0,0,0,.3);padding:13px;font-size:.77em;color:#a8e6cf;font-family:'Monaco','Courier New',monospace;overflow-x:auto;white-space:pre;line-height:1.6}
        .arch-title{font-size:1.25em;font-weight:700;color:rgba(255,255,255,.75);margin-bottom:18px;text-align:center}
        .agrid{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:14px;margin-bottom:46px}
        .acard{background:rgba(18,18,38,.75);backdrop-filter:blur(10px);border:1px solid rgba(255,255,255,.07);border-radius:16px;padding:22px;transition:all .3s}
        .acard:hover{border-color:rgba(167,139,250,.4);transform:translateY(-4px);box-shadow:0 12px 28px rgba(102,126,234,.15)}
        .aico{font-size:28px;margin-bottom:11px}
        .atitle{font-size:.97em;font-weight:700;color:#fff;margin-bottom:7px}
        .adesc{font-size:.82em;color:rgba(255,255,255,.42);line-height:1.6;margin-bottom:11px}
        .acode{font-size:.72em;color:#a78bfa;font-family:'Monaco',monospace;background:rgba(167,139,250,.1);padding:3px 8px;border-radius:6px;display:inline-block}
        .sstack{display:flex;flex-wrap:wrap;gap:9px;justify-content:center;margin-bottom:56px}
        .spill{padding:6px 15px;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.09);border-radius:20px;font-size:.82em;color:rgba(255,255,255,.55)}
        .footer{position:relative;z-index:1;text-align:center;padding:28px 24px;border-top:1px solid rgba(255,255,255,.06);color:rgba(255,255,255,.28);font-size:.88em;display:flex;flex-direction:column;gap:9px;align-items:center}
        .ghlink{color:#a78bfa;text-decoration:none;font-weight:600;transition:color .2s}
        .ghlink:hover{color:#c4b5fd}
      `}</style>
    </>
  )
}
