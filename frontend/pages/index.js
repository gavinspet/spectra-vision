import { useState, useEffect, useRef, useCallback } from 'react'
import Head from 'next/head'
import Script from 'next/script'

const API_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8080'
const MODEL_CDN = 'https://cdn.jsdelivr.net/npm/face-api.js@0.22.2/weights'

const EMOTION_MAP = {
  happy:     { label: 'Happy',     icon: '😊', color: '#FFD93D' },
  sad:       { label: 'Sad',       icon: '😢', color: '#6C5CE7' },
  angry:     { label: 'Angry',     icon: '😠', color: '#E74C3C' },
  surprised: { label: 'Surprised', icon: '😮', color: '#FD79A8' },
  neutral:   { label: 'Neutral',   icon: '😐', color: '#74B9FF' },
  fearful:   { label: 'Fearful',   icon: '😨', color: '#A29BFE' },
  disgusted: { label: 'Disgusted', icon: '🤢', color: '#00B894' },
}

const DEMO_EMOTIONS = Object.values(EMOTION_MAP)

const MODELS = [
  { value: 'emotion-v1', label: 'Emotion V1', desc: 'Fast · Mock', badge: 'FAST' },
  { value: 'emotion-v2', label: 'Emotion V2', desc: 'Advanced', badge: 'PRO' },
  { value: 'mock',       label: 'Mock',        desc: 'Testing', badge: 'TEST' },
  { value: 'advanced',   label: 'Advanced',    desc: 'ML Engine', badge: 'ML' },
]

let faceModelsLoaded = false

async function loadModels() {
  if (faceModelsLoaded) return true
  if (typeof window === 'undefined' || !window.faceapi) return false
  try {
    await window.faceapi.nets.tinyFaceDetector.loadFromUri(MODEL_CDN)
    await window.faceapi.nets.faceExpressionNet.loadFromUri(MODEL_CDN)
    faceModelsLoaded = true
    return true
  } catch (e) {
    return false
  }
}

function simulateDemo(modelId, imageData) {
  const hash = imageData.split('').reduce((a, c) => a + c.charCodeAt(0), 0)
  const emotion = DEMO_EMOTIONS[hash % DEMO_EMOTIONS.length]
  const conf = modelId === 'emotion-v1' || modelId === 'mock' ? 0.94 : 0.75 + (hash % 24) / 100
  return {
    classLabel: emotion.label,
    classId: DEMO_EMOTIONS.indexOf(emotion),
    confidence: parseFloat(conf.toFixed(4)),
    inferenceTimeMs: modelId === 'emotion-v1' || modelId === 'mock' ? 42 : 20 + (hash % 80),
    timestamp: new Date().toISOString().slice(0, 19),
    source: 'demo'
  }
}

function CameraView() {
  const videoRef = useRef(null)
  const canvasRef = useRef(null)
  const rafRef = useRef(null)
  const streamRef = useRef(null)
  const [camState, setCamState] = useState('idle')
  const [liveEmotion, setLiveEmotion] = useState(null)
  const [faceCount, setFaceCount] = useState(0)
  const [fps, setFps] = useState(0)
  const fpsRef = useRef({ frames: 0, last: Date.now() })

  const stopCamera = useCallback(() => {
    if (rafRef.current) cancelAnimationFrame(rafRef.current)
    if (streamRef.current) streamRef.current.getTracks().forEach(t => t.stop())
    streamRef.current = null
    setCamState('idle')
    setLiveEmotion(null)
    setFaceCount(0)
    if (canvasRef.current) {
      const ctx = canvasRef.current.getContext('2d')
      ctx.clearRect(0, 0, canvasRef.current.width, canvasRef.current.height)
    }
  }, [])

  const detect = useCallback(async () => {
    const video = videoRef.current
    const canvas = canvasRef.current
    if (!video || !canvas || !window.faceapi || video.paused || video.ended) return

    const w = video.videoWidth
    const h = video.videoHeight
    if (!w || !h) { rafRef.current = requestAnimationFrame(detect); return }

    canvas.width = w
    canvas.height = h

    const results = await window.faceapi
      .detectAllFaces(video, new window.faceapi.TinyFaceDetectorOptions({ inputSize: 320, scoreThreshold: 0.5 }))
      .withFaceExpressions()

    const ctx = canvas.getContext('2d')
    ctx.clearRect(0, 0, w, h)

    setFaceCount(results.length)

    if (results.length > 0) {
      const scaleX = w / video.videoWidth
      const scaleY = h / video.videoHeight

      results.forEach(det => {
        const box = det.detection.box
        const bx = box.x * scaleX
        const by = box.y * scaleY
        const bw = box.width * scaleX
        const bh = box.height * scaleY

        const sorted = Object.entries(det.expressions).sort((a, b) => b[1] - a[1])
        const [emoKey, conf] = sorted[0]
        const emo = EMOTION_MAP[emoKey] || { label: emoKey, icon: '🎭', color: '#667eea' }

        setLiveEmotion({ ...emo, confidence: conf })

        // glow shadow
        ctx.shadowColor = emo.color
        ctx.shadowBlur = 18

        // bounding box
        ctx.strokeStyle = emo.color
        ctx.lineWidth = 3
        ctx.strokeRect(bx, by, bw, bh)
        ctx.shadowBlur = 0

        // corner dots
        const cs = 12
        ctx.fillStyle = emo.color
        ;[[bx,by],[bx+bw,by],[bx,by+bh],[bx+bw,by+bh]].forEach(([cx,cy]) => {
          ctx.beginPath()
          ctx.arc(cx, cy, cs/2, 0, Math.PI*2)
          ctx.fill()
        })

        // label bg
        const labelText = `${emo.icon}  ${emo.label}  ${(conf*100).toFixed(0)}%`
        ctx.font = 'bold 16px -apple-system, sans-serif'
        const tw = ctx.measureText(labelText).width
        const lx = bx
        const ly = by > 42 ? by - 40 : by + bh + 4

        ctx.fillStyle = emo.color + 'DD'
        ctx.beginPath()
        ctx.roundRect(lx, ly, tw + 20, 34, 8)
        ctx.fill()

        ctx.fillStyle = '#fff'
        ctx.fillText(labelText, lx + 10, ly + 22)

        // mini confidence bars for all emotions
        const barX = bx + bw + 10
        if (barX + 120 < w) {
          sorted.slice(0, 4).forEach(([k, v], i) => {
            const e2 = EMOTION_MAP[k] || { color: '#667eea' }
            const by2 = by + i * 22
            ctx.fillStyle = 'rgba(0,0,0,0.5)'
            ctx.beginPath()
            ctx.roundRect(barX, by2, 110, 16, 4)
            ctx.fill()
            ctx.fillStyle = e2.color
            ctx.beginPath()
            ctx.roundRect(barX, by2, Math.max(4, v * 110), 16, 4)
            ctx.fill()
            ctx.fillStyle = '#fff'
            ctx.font = '10px -apple-system'
            ctx.fillText(`${k} ${(v*100).toFixed(0)}%`, barX + 4, by2 + 11)
          })
        }
      })
    }

    // fps
    fpsRef.current.frames++
    const now = Date.now()
    if (now - fpsRef.current.last >= 1000) {
      setFps(fpsRef.current.frames)
      fpsRef.current = { frames: 0, last: now }
    }

    rafRef.current = requestAnimationFrame(detect)
  }, [])

  const startCamera = useCallback(async () => {
    setCamState('loading')
    try {
      const ok = await loadModels()
      if (!ok) { setCamState('error'); return }

      const stream = await navigator.mediaDevices.getUserMedia({
        video: { width: { ideal: 640 }, height: { ideal: 480 }, facingMode: 'user' }
      })
      streamRef.current = stream
      videoRef.current.srcObject = stream
      await videoRef.current.play()
      setCamState('running')
      rafRef.current = requestAnimationFrame(detect)
    } catch (err) {
      setCamState('error')
    }
  }, [detect])

  useEffect(() => () => stopCamera(), [stopCamera])

  return (
    <div className="cam-container">
      <div className="cam-viewport">
        <video ref={videoRef} className="cam-video" muted playsInline />
        <canvas ref={canvasRef} className="cam-canvas" />

        {camState === 'idle' && (
          <div className="cam-overlay">
            <div className="cam-start-icon">📷</div>
            <p className="cam-start-title">Live Emotion Detection</p>
            <p className="cam-start-sub">Real-time face tracking with bounding boxes powered by face-api.js</p>
            <button className="cam-btn" onClick={startCamera}>Start Camera</button>
          </div>
        )}

        {camState === 'loading' && (
          <div className="cam-overlay">
            <div className="cam-loading-ring" />
            <p>Loading face detection models...</p>
            <p className="cam-start-sub">Downloading ~1MB from CDN</p>
          </div>
        )}

        {camState === 'error' && (
          <div className="cam-overlay">
            <div style={{fontSize:'48px'}}>⚠️</div>
            <p>Camera access denied or models failed to load</p>
            <button className="cam-btn" onClick={() => setCamState('idle')}>Try Again</button>
          </div>
        )}

        {camState === 'running' && (
          <div className="cam-hud">
            <div className="hud-item">🎯 {faceCount} face{faceCount !== 1 ? 's' : ''}</div>
            <div className="hud-item">⚡ {fps} fps</div>
            {liveEmotion && (
              <div className="hud-emotion" style={{borderColor: liveEmotion.color, color: liveEmotion.color}}>
                {liveEmotion.icon} {liveEmotion.label} · {(liveEmotion.confidence*100).toFixed(0)}%
              </div>
            )}
          </div>
        )}
      </div>

      {camState === 'running' && (
        <div className="cam-controls">
          {liveEmotion && (
            <div className="live-result" style={{'--lc': liveEmotion.color}}>
              <span className="live-icon">{liveEmotion.icon}</span>
              <div>
                <div className="live-label">{liveEmotion.label}</div>
                <div className="live-conf">
                  <div className="live-bar"><div className="live-fill" style={{width:`${liveEmotion.confidence*100}%`, background: liveEmotion.color}} /></div>
                  <span>{(liveEmotion.confidence*100).toFixed(1)}%</span>
                </div>
              </div>
            </div>
          )}
          <button className="stop-btn" onClick={stopCamera}>⏹ Stop Camera</button>
        </div>
      )}
    </div>
  )
}

export default function Home() {
  const [tab, setTab] = useState('camera')
  const [modelId, setModelId] = useState('emotion-v1')
  const [imageData, setImageData] = useState('test-image-data')
  const [result, setResult] = useState(null)
  const [loading, setLoading] = useState(false)
  const [serverStatus, setServerStatus] = useState('checking')
  const [demoTab, setDemoTab] = useState('demo')
  const [animRes, setAnimRes] = useState(false)
  const [scriptLoaded, setScriptLoaded] = useState(false)

  useEffect(() => {
    checkServer()
    // Poll for face-api.js since beforeInteractive onLoad is unreliable
    const interval = setInterval(() => {
      if (typeof window !== 'undefined' && window.faceapi) {
        setScriptLoaded(true)
        clearInterval(interval)
      }
    }, 200)
    return () => clearInterval(interval)
  }, [])

  const checkServer = async () => {
    try {
      const ctrl = new AbortController()
      const t = setTimeout(() => ctrl.abort(), 5000)
      const res = await fetch(`${API_URL}/health`, { signal: ctrl.signal })
      clearTimeout(t)
      setServerStatus(res.ok ? 'online' : 'offline')
    } catch { setServerStatus('offline') }
  }

  const handleDetect = async (e) => {
    e.preventDefault()
    setLoading(true); setResult(null); setAnimRes(false)
    if (demoTab === 'demo' || serverStatus !== 'online') {
      await new Promise(r => setTimeout(r, 1000 + Math.random() * 600))
      setResult({ ok: true, data: simulateDemo(modelId, imageData) })
    } else {
      try {
        const res = await fetch(`${API_URL}/api/v1/emotion`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ modelId, imageBase64: imageData })
        })
        setResult({ ok: res.ok, data: await res.json() })
      } catch {
        setResult({ ok: true, data: { ...simulateDemo(modelId, imageData), source: 'demo-fallback' } })
      }
    }
    setLoading(false)
    setTimeout(() => setAnimRes(true), 50)
  }

  const curEmo = result?.ok ? DEMO_EMOTIONS.find(e => e.label === result.data.classLabel) : null

  return (
    <>
      <Head>
        <title>Spectra Vision - Live AI Emotion Detection</title>
        <meta name="description" content="Real-time emotion detection with face bounding boxes, powered by C++ backend" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
      </Head>
      <Script
        src="https://cdn.jsdelivr.net/npm/face-api.js@0.22.2/dist/face-api.min.js"
        strategy="afterInteractive"
      />

      <div className="page">
        <div className="orb orb1" /><div className="orb orb2" /><div className="orb orb3" />

        <header className="hdr">
          <div className="hdr-inner">
            <div className="brand">
              <span className="brand-icon">🎭</span>
              <div>
                <div className="brand-name">Spectra Vision</div>
                <div className="brand-sub">Live AI Emotion Detection</div>
              </div>
            </div>
            <div className="hdr-right">
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
            <p className="hero-p">Live camera feed with face bounding boxes · Strategy, Factory, Singleton &amp; Dependency Injection patterns · Sub-100ms inference</p>
          </div>

          <div className="main-tabs">
            <button className={`mtab ${tab==='camera'?'mtab-on':''}`} onClick={() => setTab('camera')}>📷 Live Camera</button>
            <button className={`mtab ${tab==='demo'?'mtab-on':''}`} onClick={() => setTab('demo')}>⚡ API Demo</button>
            <button className={`mtab ${tab==='arch'?'mtab-on':''}`} onClick={() => setTab('arch')}>🏗 Architecture</button>
          </div>

          {tab === 'camera' && (
            <div className="panel">
              <div className="panel-hdr">
                <div className="panel-title">Live Camera Detection</div>
                <div className="panel-sub">Real-time face tracking · Emotion classification · Bounding boxes</div>
              </div>
              {scriptLoaded ? <CameraView /> : (
                <div className="loading-faceapi">
                  <div className="lring" />
                  <p>Loading face detection library...</p>
                </div>
              )}
            </div>
          )}

          {tab === 'demo' && (
            <div className="grid2">
              <div className="panel">
                <div className="panel-title">Detect Emotion</div>
                <div className="dtabs">
                  <button className={`dtab ${demoTab==='demo'?'dtab-on':''}`} onClick={() => setDemoTab('demo')}>⚡ Demo</button>
                  <button className={`dtab ${demoTab==='live'?'dtab-on':''}`} onClick={() => setDemoTab('live')}>
                    🌐 Live API{serverStatus !== 'online' && <span className="tab-off"> · offline</span>}
                  </button>
                </div>
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
                    <div className="fhint">{demoTab==='demo' ? '⚡ Demo mode — simulated locally' : serverStatus==='online' ? '🌐 Connected to live C++ API' : '⚡ Backend offline — using demo mode'}</div>
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
                  <div className={`res ${animRes?'res-in':''}`}>
                    {result.ok && curEmo ? (
                      <>
                        <div className="ehero" style={{'--ec': curEmo.color}}>
                          <div className="eico">{curEmo.icon}</div>
                          <div className="elabel">{curEmo.label}</div>
                          <div className="cbar-wrap">
                            <div className="cbar-track"><div className="cbar-fill" style={{width: animRes ? `${result.data.confidence*100}%` : '0%', background: curEmo.color}} /></div>
                            <span className="cpct">{(result.data.confidence*100).toFixed(1)}%</span>
                          </div>
                        </div>
                        <div className="mgrid2">
                          <div className="mbox"><span className="mkey">Inference</span><span className="mval">{result.data.inferenceTimeMs}ms</span></div>
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
          )}

          {tab === 'arch' && (
            <>
              <div className="agrid">
                {[
                  {ico:'🔄', title:'Strategy Pattern', desc:'Swappable detection algorithms at runtime — MockDetector & AdvancedDetector share a common interface', code:'EmotionDetectionStrategy'},
                  {ico:'🏭', title:'Factory Pattern', desc:'EmotionModelFactory creates the right detector from modelId without tight client coupling', code:'EmotionModelFactory::create()'},
                  {ico:'♾️', title:'Singleton Pattern', desc:"Thread-safe Meyer's Singleton logger — single global instance across all translation units", code:'Logger::getInstance()'},
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
                {['CMake 3.20','cpp-httplib','nlohmann/json','Docker','GitHub Actions','face-api.js','Next.js 14','Vercel'].map(t => <span key={t} className="spill">{t}</span>)}
              </div>
            </>
          )}
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
        .main{position:relative;z-index:1;max-width:1100px;margin:0 auto;padding:50px 24px 40px}
        .hero{text-align:center;margin-bottom:40px}
        .hero-h1{font-size:clamp(1.8em,4vw,2.8em);font-weight:800;line-height:1.2;margin-bottom:12px;color:#fff}
        .hero-grad{display:block;background:linear-gradient(135deg,#a78bfa 0%,#60a5fa 50%,#f0abfc 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text}
        .hero-p{max-width:650px;margin:0 auto;color:rgba(255,255,255,.45);font-size:1em;line-height:1.7}
        .main-tabs{display:flex;gap:6px;margin-bottom:26px;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.08);border-radius:14px;padding:5px;width:fit-content}
        .mtab{padding:10px 22px;border-radius:10px;font-size:.9em;font-weight:600;border:none;cursor:pointer;background:transparent;color:rgba(255,255,255,.45);transition:all .2s;display:flex;align-items:center;gap:6px;width:auto;transform:none}
        .mtab:hover{color:#fff;background:rgba(255,255,255,.06);box-shadow:none;transform:none}
        .mtab-on{background:linear-gradient(135deg,#667eea,#764ba2)!important;color:#fff!important;box-shadow:0 4px 15px rgba(102,126,234,.3)}
        .panel{background:rgba(18,18,38,.8);backdrop-filter:blur(20px);border:1px solid rgba(255,255,255,.08);border-radius:20px;padding:28px;margin-bottom:24px}
        .panel-hdr{margin-bottom:22px;padding-bottom:16px;border-bottom:1px solid rgba(255,255,255,.07)}
        .panel-title{font-size:1.1em;font-weight:700;color:rgba(255,255,255,.8);margin-bottom:4px}
        .panel-sub{font-size:.82em;color:rgba(255,255,255,.35)}
        .grid2{display:grid;grid-template-columns:1fr 1fr;gap:22px;margin-bottom:24px}
        @media(max-width:768px){.grid2{grid-template-columns:1fr}}
        .dtabs{display:flex;gap:5px;margin-bottom:20px;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.07);border-radius:10px;padding:4px;width:fit-content}
        .dtab{padding:7px 16px;border-radius:7px;font-size:.82em;font-weight:600;border:none;cursor:pointer;background:transparent;color:rgba(255,255,255,.4);transition:all .2s;display:flex;align-items:center;gap:5px;width:auto;transform:none}
        .dtab:hover{color:#fff;background:rgba(255,255,255,.06);box-shadow:none;transform:none}
        .dtab-on{background:linear-gradient(135deg,#667eea,#764ba2)!important;color:#fff!important}
        .tab-off{font-size:.78em;color:rgba(255,255,255,.3);font-weight:400}
        .field{margin-bottom:20px}
        .flabel{display:block;font-size:.78em;font-weight:600;color:rgba(255,255,255,.4);text-transform:uppercase;letter-spacing:.8px;margin-bottom:9px}
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
        .rpanel{min-height:350px}
        .empty{display:flex;flex-direction:column;align-items:center;justify-content:center;height:260px;gap:11px;color:rgba(255,255,255,.28);text-align:center;font-size:.92em}
        .empty-ico{font-size:46px;opacity:.35}
        .lring{width:46px;height:46px;border:3px solid rgba(167,139,250,.2);border-top-color:#a78bfa;border-radius:50%;animation:spin 1s linear infinite}
        .lsub{font-size:.76em;color:rgba(255,255,255,.2)}
        .res{opacity:0;transform:translateY(10px);transition:all .4s ease}
        .res-in{opacity:1;transform:translateY(0)}
        .ehero{text-align:center;padding:24px 16px;background:rgba(255,255,255,.03);border-radius:14px;margin-bottom:14px;border:1px solid rgba(255,255,255,.07)}
        .eico{font-size:60px;margin-bottom:8px}
        .elabel{font-size:1.6em;font-weight:800;color:var(--ec);margin-bottom:14px}
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
        .loading-faceapi{display:flex;flex-direction:column;align-items:center;justify-content:center;height:300px;gap:16px;color:rgba(255,255,255,.4)}
        .cam-container{display:flex;flex-direction:column;gap:16px}
        .cam-viewport{position:relative;width:100%;aspect-ratio:4/3;background:#000;border-radius:14px;overflow:hidden;border:1px solid rgba(255,255,255,.1);max-height:480px}
        .cam-video{width:100%;height:100%;object-fit:cover;transform:scaleX(-1)}
        .cam-canvas{position:absolute;top:0;left:0;width:100%;height:100%;transform:scaleX(-1)}
        .cam-overlay{position:absolute;inset:0;display:flex;flex-direction:column;align-items:center;justify-content:center;background:rgba(8,8,24,.85);gap:14px;color:#e0e0ff;text-align:center;padding:24px}
        .cam-start-icon{font-size:56px;opacity:.7}
        .cam-start-title{font-size:1.3em;font-weight:700;color:#fff}
        .cam-start-sub{font-size:.88em;color:rgba(255,255,255,.4);max-width:320px;line-height:1.5}
        .cam-btn{padding:13px 28px;background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;border:none;border-radius:12px;font-size:1em;font-weight:700;cursor:pointer;transition:all .3s;width:auto;transform:none}
        .cam-btn:hover{transform:translateY(-2px);box-shadow:0 10px 24px rgba(102,126,234,.4)}
        .cam-loading-ring{width:52px;height:52px;border:3px solid rgba(167,139,250,.2);border-top-color:#a78bfa;border-radius:50%;animation:spin 1s linear infinite}
        .cam-hud{position:absolute;top:12px;left:12px;right:12px;display:flex;gap:8px;flex-wrap:wrap;align-items:center}
        .hud-item{padding:5px 12px;background:rgba(0,0,0,.65);backdrop-filter:blur(10px);border-radius:20px;font-size:.78em;font-weight:600;color:rgba(255,255,255,.8);border:1px solid rgba(255,255,255,.1)}
        .hud-emotion{padding:5px 14px;background:rgba(0,0,0,.7);backdrop-filter:blur(10px);border-radius:20px;font-size:.82em;font-weight:700;border:1px solid;margin-left:auto}
        .cam-controls{display:flex;align-items:center;gap:14px;flex-wrap:wrap}
        .live-result{display:flex;align-items:center;gap:14px;flex:1;background:rgba(18,18,38,.8);border:1px solid rgba(255,255,255,.08);border-radius:14px;padding:14px 18px}
        .live-icon{font-size:36px}
        .live-label{font-size:1.15em;font-weight:700;color:#fff;margin-bottom:6px}
        .live-conf{display:flex;align-items:center;gap:8px}
        .live-bar{flex:1;min-width:80px;height:6px;background:rgba(255,255,255,.1);border-radius:10px;overflow:hidden}
        .live-fill{height:100%;border-radius:10px;transition:width .4s ease}
        .stop-btn{padding:12px 20px;background:rgba(239,68,68,.15);color:#f87171;border:1px solid rgba(239,68,68,.3);border-radius:12px;font-size:.9em;font-weight:600;cursor:pointer;transition:all .2s;white-space:nowrap;transform:none;width:auto;box-shadow:none}
        .stop-btn:hover{background:rgba(239,68,68,.25);transform:none;box-shadow:none}
        .agrid{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:14px;margin-bottom:40px}
        .acard{background:rgba(18,18,38,.8);border:1px solid rgba(255,255,255,.07);border-radius:16px;padding:22px;transition:all .3s}
        .acard:hover{border-color:rgba(167,139,250,.4);transform:translateY(-4px);box-shadow:0 12px 28px rgba(102,126,234,.15)}
        .aico{font-size:28px;margin-bottom:11px}
        .atitle{font-size:.97em;font-weight:700;color:#fff;margin-bottom:7px}
        .adesc{font-size:.82em;color:rgba(255,255,255,.42);line-height:1.6;margin-bottom:11px}
        .acode{font-size:.72em;color:#a78bfa;font-family:'Monaco',monospace;background:rgba(167,139,250,.1);padding:3px 8px;border-radius:6px;display:inline-block}
        .sstack{display:flex;flex-wrap:wrap;gap:9px;justify-content:center;margin-bottom:40px}
        .spill{padding:6px 15px;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.09);border-radius:20px;font-size:.82em;color:rgba(255,255,255,.55)}
        .footer{position:relative;z-index:1;text-align:center;padding:28px 24px;border-top:1px solid rgba(255,255,255,.06);color:rgba(255,255,255,.28);font-size:.88em;display:flex;flex-direction:column;gap:9px;align-items:center}
        .ghlink{color:#a78bfa;text-decoration:none;font-weight:600;transition:color .2s}
        .ghlink:hover{color:#c4b5fd}
      `}</style>
    </>
  )
}
