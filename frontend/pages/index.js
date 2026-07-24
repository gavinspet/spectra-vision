import { useState, useEffect, useRef, useCallback } from 'react'
import Head from 'next/head'

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

let faceModelsLoaded = false

async function loadModels() {
  if (faceModelsLoaded) return true
  if (typeof window === 'undefined' || !window.faceapi) return false
  try {
    await window.faceapi.nets.tinyFaceDetector.loadFromUri(MODEL_CDN)
    await window.faceapi.nets.faceExpressionNet.loadFromUri(MODEL_CDN)
    faceModelsLoaded = true
    return true
  } catch { return false }
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
  }, [])

  const detect = useCallback(async () => {
    const video = videoRef.current
    const canvas = canvasRef.current
    if (!video || !canvas || !window.faceapi || video.paused) return
    const w = video.videoWidth, h = video.videoHeight
    if (!w || !h) { rafRef.current = requestAnimationFrame(detect); return }
    canvas.width = w; canvas.height = h
    const results = await window.faceapi
      .detectAllFaces(video, new window.faceapi.TinyFaceDetectorOptions({ inputSize: 320, scoreThreshold: 0.5 }))
      .withFaceExpressions()
    const ctx = canvas.getContext('2d')
    ctx.clearRect(0, 0, w, h)
    setFaceCount(results.length)
    if (results.length > 0) {
      results.forEach(det => {
        const { x, y, width, height } = det.detection.box
        const sorted = Object.entries(det.expressions).sort((a, b) => b[1] - a[1])
        const [emoKey, conf] = sorted[0]
        const emo = EMOTION_MAP[emoKey] || { label: emoKey, icon: '🎭', color: '#667eea' }
        setLiveEmotion({ ...emo, confidence: conf })
        ctx.shadowColor = emo.color; ctx.shadowBlur = 20
        ctx.strokeStyle = emo.color; ctx.lineWidth = 2.5
        ctx.strokeRect(x, y, width, height)
        ctx.shadowBlur = 0
        const cs = 10
        ctx.fillStyle = emo.color
        ;[[x,y],[x+width,y],[x,y+height],[x+width,y+height]].forEach(([cx,cy]) => {
          ctx.beginPath(); ctx.arc(cx, cy, cs/2, 0, Math.PI*2); ctx.fill()
        })
        const labelText = `${emo.icon}  ${emo.label}  ${(conf*100).toFixed(0)}%`
        ctx.font = 'bold 14px -apple-system, sans-serif'
        const tw = ctx.measureText(labelText).width
        const lx = x, ly = y > 40 ? y - 38 : y + height + 4
        ctx.fillStyle = emo.color + 'EE'
        ctx.beginPath()
        if (ctx.roundRect) { ctx.roundRect(lx, ly, tw + 18, 30, 6) } else { ctx.rect(lx, ly, tw + 18, 30) }
        ctx.fill()
        ctx.fillStyle = '#fff'; ctx.fillText(labelText, lx + 9, ly + 20)
        const barX = x + width + 8
        if (barX + 100 < w) {
          sorted.slice(0, 5).forEach(([k, v], i) => {
            const e2 = EMOTION_MAP[k] || { color: '#667eea' }
            const by2 = y + i * 20
            ctx.fillStyle = 'rgba(0,0,0,0.55)'
            if (ctx.roundRect) { ctx.beginPath(); ctx.roundRect(barX, by2, 100, 15, 3); ctx.fill() }
            else { ctx.fillRect(barX, by2, 100, 15) }
            ctx.fillStyle = e2.color
            if (ctx.roundRect) { ctx.beginPath(); ctx.roundRect(barX, by2, Math.max(4, v*100), 15, 3); ctx.fill() }
            else { ctx.fillRect(barX, by2, Math.max(4, v*100), 15) }
            ctx.fillStyle = '#fff'; ctx.font = '9px -apple-system'
            ctx.fillText(`${k} ${(v*100).toFixed(0)}%`, barX + 3, by2 + 10)
          })
        }
      })
    }
    fpsRef.current.frames++
    const now = Date.now()
    if (now - fpsRef.current.last >= 1000) {
      setFps(fpsRef.current.frames); fpsRef.current = { frames: 0, last: now }
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
    } catch { setCamState('error') }
  }, [detect])

  useEffect(() => () => stopCamera(), [stopCamera])

  return (
    <div className="cam-wrap">
      <div className="cam-vp">
        <video ref={videoRef} className="cam-vid" muted playsInline />
        <canvas ref={canvasRef} className="cam-cvs" />
        {camState === 'idle' && (
          <div className="cam-overlay">
            <div className="cam-icon-wrap"><span className="cam-big-icon">📷</span></div>
            <h3 className="cam-title">Live Emotion Detection</h3>
            <p className="cam-desc">Real-time face tracking with bounding boxes using TinyFaceDetector · All processing runs in your browser</p>
            <button className="primary-btn" onClick={startCamera}>
              <span>Start Camera</span>
              <span className="btn-arrow">→</span>
            </button>
          </div>
        )}
        {camState === 'loading' && (
          <div className="cam-overlay">
            <div className="loader-ring" />
            <p className="cam-title" style={{fontSize:'1em'}}>Loading detection models...</p>
            <p className="cam-desc">Downloading face detection weights (~1MB)</p>
            <div className="progress-dots"><span/><span/><span/></div>
          </div>
        )}
        {camState === 'error' && (
          <div className="cam-overlay">
            <span style={{fontSize:'48px'}}>⚠️</span>
            <p className="cam-title" style={{fontSize:'1em'}}>Could not start camera</p>
            <p className="cam-desc">Check camera permissions or browser support</p>
            <button className="primary-btn" onClick={() => setCamState('idle')}>Try Again</button>
          </div>
        )}
        {camState === 'running' && (
          <div className="cam-hud">
            <div className="hud-pill">👤 {faceCount} face{faceCount !== 1 ? 's' : ''}</div>
            <div className="hud-pill">⚡ {fps} fps</div>
            {liveEmotion && (
              <div className="hud-emo" style={{borderColor: liveEmotion.color + '80', color: liveEmotion.color}}>
                {liveEmotion.icon} {liveEmotion.label} · {(liveEmotion.confidence*100).toFixed(0)}%
              </div>
            )}
          </div>
        )}
      </div>
      {camState === 'running' && (
        <div className="cam-footer">
          {liveEmotion && (
            <div className="live-bar-wrap" style={{'--lc': liveEmotion.color}}>
              <span className="live-emo-icon">{liveEmotion.icon}</span>
              <div className="live-emo-info">
                <div className="live-emo-name">{liveEmotion.label}</div>
                <div className="live-emo-bar">
                  <div className="live-emo-fill" style={{width:`${liveEmotion.confidence*100}%`, background: liveEmotion.color}} />
                </div>
              </div>
              <span className="live-emo-pct">{(liveEmotion.confidence*100).toFixed(1)}%</span>
              <button className="stop-btn" onClick={stopCamera}>Stop</button>
            </div>
          )}
        </div>
      )}
    </div>
  )
}

const ARCH_PATTERNS = [
  {
    icon: '🔄', title: 'Strategy Pattern',
    desc: 'Defines a family of algorithms, encapsulates each one, and makes them interchangeable. Detectors can be swapped at runtime without modifying client code.',
    code: `class EmotionDetectionStrategy {
public:
  virtual EmotionResult detect(
    const std::string& imageData
  ) = 0;
  virtual ~EmotionDetectionStrategy() = default;
};

// Concrete strategies
class MockEmotionDetector     : public EmotionDetectionStrategy {};
class AdvancedEmotionDetector : public EmotionDetectionStrategy {};`,
    tag: 'Behavioral'
  },
  {
    icon: '🏭', title: 'Factory Pattern',
    desc: 'Centralizes object creation logic. The factory decides which concrete implementation to instantiate based on the modelId, hiding construction details from callers.',
    code: `class EmotionModelFactory {
public:
  static std::shared_ptr<EmotionDetectionStrategy>
  create(const std::string& modelId) {
    if (modelId == "emotion-v1") return
      std::make_shared<MockEmotionDetector>();
    if (modelId == "advanced") return
      std::make_shared<AdvancedEmotionDetector>();
    throw ModelException("Unknown model: " + modelId);
  }
};`,
    tag: 'Creational'
  },
  {
    icon: '♾️', title: 'Singleton Pattern',
    desc: "Meyer's Singleton ensures a single Logger instance across all translation units. Thread-safe by modern C++ guarantee — no mutexes required.",
    code: `class Logger {
public:
  static Logger& getInstance() {
    static Logger instance; // thread-safe guarantee
    return instance;
  }
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
private:
  Logger() = default;
};`,
    tag: 'Creational'
  },
  {
    icon: '💉', title: 'Dependency Injection',
    desc: 'APIHandler receives its detector dependency from outside rather than creating it — enables unit testing, loose coupling, and the Open/Closed Principle.',
    code: `class APIHandler {
public:
  // Dependency injected via constructor
  explicit APIHandler(
    std::shared_ptr<EmotionDetectionStrategy> detector
  ) : m_detector(std::move(detector)) {}

  json handleEmotionDetection(const json& req) {
    return m_detector->detect(req["imageBase64"]);
  }
private:
  std::shared_ptr<EmotionDetectionStrategy> m_detector;
};`,
    tag: 'Structural'
  },
]

const METRICS = [
  { label: 'Avg Inference', value: '42ms', icon: '⚡' },
  { label: 'Language', value: 'C++', icon: '🔧' },
  { label: 'API Endpoints', value: '3', icon: '🔗' },
  { label: 'Architecture', value: 'MVC', icon: '🏗' },
]

export default function Home() {
  const [tab, setTab] = useState('camera')
  const [serverStatus, setServerStatus] = useState('checking')
  const [faceApiReady, setFaceApiReady] = useState(false)
  const [openPattern, setOpenPattern] = useState(null)

  useEffect(() => {
    checkServer()
    // Dynamically inject face-api.js script — more reliable than next/script
    // Check if already loaded (e.g. cached)
    if (typeof window !== 'undefined' && window.faceapi) {
      setFaceApiReady(true)
      return
    }
    const script = document.createElement('script')
    script.src = 'https://cdn.jsdelivr.net/npm/face-api.js@0.22.2/dist/face-api.min.js'
    script.async = true
    script.onload = () => setFaceApiReady(true)
    script.onerror = () => console.warn('face-api.js failed to load from CDN')
    document.head.appendChild(script)
    // Fallback poll in case onload doesn't fire
    const poll = setInterval(() => {
      if (window.faceapi) { setFaceApiReady(true); clearInterval(poll) }
    }, 300)
    return () => {
      clearInterval(poll)
      if (document.head.contains(script)) document.head.removeChild(script)
    }
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

  return (
    <>
      <Head>
        <title>Spectra Vision — Live AI Emotion Detection</title>
        <meta name="description" content="Real-time face emotion detection with bounding boxes. C++ backend with Strategy, Factory, Singleton and DI patterns." />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
      </Head>

      <div className="page">
        <div className="orb orb1" /><div className="orb orb2" /><div className="orb orb3" />

        {/* ── HEADER ── */}
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
              <a href="https://github.com/gavinspet/spectra-vision" target="_blank" rel="noopener noreferrer" className="gh-btn">
                <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor"><path d="M12 0C5.37 0 0 5.37 0 12c0 5.31 3.435 9.795 8.205 11.385.6.105.825-.255.825-.57 0-.285-.015-1.23-.015-2.235-3.015.555-3.795-.735-4.035-1.41-.135-.345-.72-1.41-1.23-1.695-.42-.225-1.02-.78-.015-.795.945-.015 1.62.87 1.845 1.23 1.08 1.815 2.805 1.305 3.495.99.105-.78.42-1.305.765-1.605-2.67-.3-5.46-1.335-5.46-5.925 0-1.305.465-2.385 1.23-3.225-.12-.3-.54-1.53.12-3.18 0 0 1.005-.315 3.3 1.23.96-.27 1.98-.405 3-.405s2.04.135 3 .405c2.295-1.56 3.3-1.23 3.3-1.23.66 1.65.24 2.88.12 3.18.765.84 1.23 1.905 1.23 3.225 0 4.605-2.805 5.625-5.475 5.925.435.375.81 1.095.81 2.22 0 1.605-.015 2.895-.015 3.3 0 .315.225.69.825.57A12.02 12.02 0 0 0 24 12c0-6.63-5.37-12-12-12z"/></svg>
                GitHub
              </a>
              <div className={`status-pill ${serverStatus}`}>
                <span className="sdot" />
                {serverStatus === 'online' ? 'API Connected' : serverStatus === 'checking' ? 'Connecting...' : 'Demo Mode'}
              </div>
            </div>
          </div>
        </header>

        <main className="main">
          {/* ── HERO ── */}
          <section className="hero">
            <div className="hero-eyebrow">Computer Vision · Machine Learning · Systems Programming</div>
            <h1 className="hero-h1">
              Emotion Detection<br/>
              <span className="hero-grad">From Camera to API</span>
            </h1>
            <p className="hero-p">
              Browser-based face detection with real-time bounding boxes, backed by a production C++ HTTP server
              implementing Strategy, Factory, Singleton and Dependency Injection design patterns.
            </p>
            <div className="hero-metrics">
              {METRICS.map(m => (
                <div key={m.label} className="metric-box">
                  <span className="metric-icon">{m.icon}</span>
                  <span className="metric-val">{m.value}</span>
                  <span className="metric-label">{m.label}</span>
                </div>
              ))}
            </div>
          </section>

          {/* ── TABS ── */}
          <div className="tab-bar">
            <button className={`tab-btn ${tab==='camera'?'tab-active':''}`} onClick={() => setTab('camera')}>
              <span className="tab-icon">📷</span>
              <span>Live Demo</span>
              <span className="tab-badge free">FREE</span>
            </button>
            <button className={`tab-btn ${tab==='api'?'tab-active':''}`} onClick={() => setTab('api')}>
              <span className="tab-icon">🔒</span>
              <span>API Demo</span>
              <span className="tab-badge paid">PREMIUM</span>
            </button>
            <button className={`tab-btn ${tab==='arch'?'tab-active':''}`} onClick={() => setTab('arch')}>
              <span className="tab-icon">🏗</span>
              <span>Architecture</span>
            </button>
          </div>

          {/* ── LIVE DEMO TAB ── */}
          {tab === 'camera' && (
            <div className="content-section">
              <div className="section-hdr">
                <div>
                  <h2 className="section-title">Live Camera Detection</h2>
                  <p className="section-sub">Real-time face tracking · Emotion classification · Animated bounding boxes · All in-browser</p>
                </div>
                <div className="trial-badge">🎁 Free Trial</div>
              </div>
              {faceApiReady
                ? <CameraView />
                : (
                  <div className="loading-models">
                    <div className="loader-ring" />
                    <p>Loading face detection library...</p>
                    <p className="loading-sub">face-api.js · TinyFaceDetector · ~1MB</p>
                  </div>
                )
              }
            </div>
          )}

          {/* ── API DEMO TAB (LOCKED) ── */}
          {tab === 'api' && (
            <div className="content-section">
              <div className="section-hdr">
                <div>
                  <h2 className="section-title">API Integration Demo</h2>
                  <p className="section-sub">Direct integration with C++ HTTP server · Real inference · JSON responses</p>
                </div>
                <div className="premium-badge">🔒 Premium</div>
              </div>
              <div className="locked-wrap">
                <div className="locked-preview" aria-hidden="true">
                  <div className="lp-row">
                    <div className="lp-panel">
                      <div className="lp-label">Detection Model</div>
                      <div className="lp-select">Emotion V2 — Advanced <span>▾</span></div>
                      <div className="lp-label" style={{marginTop:'16px'}}>Image Input</div>
                      <div className="lp-textarea">/9j/4AAQSkZJRgABAQAAAQABAAD/2wBD...</div>
                      <div className="lp-btn">⚡ Analyze via C++ Backend</div>
                    </div>
                    <div className="lp-panel">
                      <div className="lp-label">Live Response</div>
                      <div className="lp-result">
                        <div className="lp-emo">😊</div>
                        <div className="lp-emo-name">Happy</div>
                        <div className="lp-conf-track"><div className="lp-conf-fill" /></div>
                        <div className="lp-json">{`{\n  "classLabel": "Happy",\n  "confidence": 0.9412,\n  "inferenceTimeMs": 42,\n  "classId": 0\n}`}</div>
                      </div>
                    </div>
                  </div>
                </div>
                <div className="lock-overlay">
                  <div className="lock-card">
                    <div className="lock-icon">🔐</div>
                    <h3 className="lock-title">Premium Feature</h3>
                    <p className="lock-desc">
                      This panel connects directly to the production C++ backend — live inference,
                      real confidence scores, and sub-50ms response times.
                    </p>
                    <div className="lock-features">
                      <div className="lock-feat">✓ Direct C++ API calls</div>
                      <div className="lock-feat">✓ Real-time inference pipeline</div>
                      <div className="lock-feat">✓ Structured JSON responses</div>
                      <div className="lock-feat">✓ Model selection &amp; switching</div>
                    </div>
                    <div className="lock-endpoints">
                      <div className="ep-row"><span className="ep-method get">GET</span><code>/health</code><span className="ep-desc">Service status</span></div>
                      <div className="ep-row"><span className="ep-method post">POST</span><code>/api/v1/emotion</code><span className="ep-desc">Detect emotion</span></div>
                      <div className="ep-row"><span className="ep-method get">GET</span><code>/api/v1/models</code><span className="ep-desc">List models</span></div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          )}

          {/* ── ARCHITECTURE TAB ── */}
          {tab === 'arch' && (
            <div className="content-section">
              <div className="section-hdr">
                <div>
                  <h2 className="section-title">System Architecture</h2>
                  <p className="section-sub">C++17 backend · CMake build system · Docker deployment · GitHub Actions CI/CD</p>
                </div>
              </div>

              {/* System diagram */}
              <div className="sys-diagram">
                <div className="sys-node browser">
                  <div className="sys-node-icon">🌐</div>
                  <div className="sys-node-label">Browser</div>
                  <div className="sys-node-sub">Next.js + face-api.js</div>
                </div>
                <div className="sys-arrow">
                  <div className="sys-arrow-line" />
                  <div className="sys-arrow-label">HTTPS / JSON</div>
                </div>
                <div className="sys-node server">
                  <div className="sys-node-icon">⚙️</div>
                  <div className="sys-node-label">C++ Server</div>
                  <div className="sys-node-sub">cpp-httplib · Port 8080</div>
                </div>
                <div className="sys-arrow">
                  <div className="sys-arrow-line" />
                  <div className="sys-arrow-label">Factory → Strategy</div>
                </div>
                <div className="sys-node engine">
                  <div className="sys-node-icon">🧠</div>
                  <div className="sys-node-label">Inference Engine</div>
                  <div className="sys-node-sub">Strategy Pattern · DI</div>
                </div>
              </div>

              {/* Design patterns */}
              <h3 className="sub-section-title">Design Patterns</h3>
              <div className="patterns-grid">
                {ARCH_PATTERNS.map((p, i) => (
                  <div
                    key={p.title}
                    className={`pattern-card ${openPattern === i ? 'pattern-open' : ''}`}
                    onClick={() => setOpenPattern(openPattern === i ? null : i)}
                  >
                    <div className="pc-header">
                      <div className="pc-left">
                        <span className="pc-icon">{p.icon}</span>
                        <div>
                          <div className="pc-title">{p.title}</div>
                          <div className="pc-tag">{p.tag}</div>
                        </div>
                      </div>
                      <span className="pc-toggle">{openPattern === i ? '−' : '+'}</span>
                    </div>
                    <p className="pc-desc">{p.desc}</p>
                    {openPattern === i && (
                      <pre className="pc-code"><code>{p.code}</code></pre>
                    )}
                  </div>
                ))}
              </div>

              {/* Build pipeline */}
              <h3 className="sub-section-title">Build & Deployment Pipeline</h3>
              <div className="pipeline">
                {[
                  { icon: '📝', label: 'Source', sub: 'C++17 · CMake 3.20' },
                  { icon: '🔨', label: 'Build', sub: 'Ninja · GCC/Clang' },
                  { icon: '✅', label: 'Test', sub: 'GitHub Actions CI' },
                  { icon: '🐳', label: 'Package', sub: 'Docker · Ubuntu 22.04' },
                  { icon: '🚀', label: 'Deploy', sub: 'Render · Auto-deploy' },
                ].map((s, i) => (
                  <div key={s.label} className="pipe-step">
                    <div className="pipe-node">
                      <span className="pipe-icon">{s.icon}</span>
                    </div>
                    {i < 4 && <div className="pipe-connector" />}
                    <div className="pipe-label">{s.label}</div>
                    <div className="pipe-sub">{s.sub}</div>
                  </div>
                ))}
              </div>

              {/* File structure */}
              <h3 className="sub-section-title">Project Structure</h3>
              <div className="file-tree">
                <pre className="tree-code">{`spectra-vision/
├── backend/
│   ├── include/
│   │   ├── emotion_detection.hpp  ← Strategy + concrete detectors
│   │   ├── model_factory.hpp      ← Factory pattern
│   │   ├── logger.hpp             ← Singleton pattern
│   │   └── api_handler.hpp        ← Dependency Injection
│   ├── src/
│   │   ├── main.cpp               ← HTTP server (cpp-httplib)
│   │   ├── emotion_detection.cpp
│   │   ├── model_factory.cpp
│   │   ├── logger.cpp
│   │   └── api_handler.cpp
│   └── CMakeLists.txt
├── frontend/
│   └── pages/index.js             ← Next.js + face-api.js
├── Dockerfile                     ← Multi-stage build
└── .github/workflows/build.yml    ← CI/CD pipeline`}</pre>
              </div>

              {/* Tech stack */}
              <h3 className="sub-section-title">Technology Stack</h3>
              <div className="tech-grid">
                {[
                  { cat: 'Backend', items: ['C++17', 'CMake 3.20', 'Ninja Build', 'cpp-httplib', 'nlohmann/json'] },
                  { cat: 'Frontend', items: ['Next.js 14', 'React 18', 'face-api.js', 'CSS-in-JS'] },
                  { cat: 'Infrastructure', items: ['Docker', 'Ubuntu 22.04', 'Render', 'Vercel', 'GitHub Actions'] },
                ].map(g => (
                  <div key={g.cat} className="tech-card">
                    <div className="tech-cat">{g.cat}</div>
                    <div className="tech-items">
                      {g.items.map(t => <span key={t} className="tech-pill">{t}</span>)}
                    </div>
                  </div>
                ))}
              </div>
            </div>
          )}
        </main>

        <footer className="footer">
          <p>Built to demonstrate production C++ architecture and design patterns</p>
          <a href="https://github.com/gavinspet/spectra-vision" target="_blank" rel="noopener noreferrer" className="footer-link">
            <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor"><path d="M12 0C5.37 0 0 5.37 0 12c0 5.31 3.435 9.795 8.205 11.385.6.105.825-.255.825-.57 0-.285-.015-1.23-.015-2.235-3.015.555-3.795-.735-4.035-1.41-.135-.345-.72-1.41-1.23-1.695-.42-.225-1.02-.78-.015-.795.945-.015 1.62.87 1.845 1.23 1.08 1.815 2.805 1.305 3.495.99.105-.78.42-1.305.765-1.605-2.67-.3-5.46-1.335-5.46-5.925 0-1.305.465-2.385 1.23-3.225-.12-.3-.54-1.53.12-3.18 0 0 1.005-.315 3.3 1.23.96-.27 1.98-.405 3-.405s2.04.135 3 .405c2.295-1.56 3.3-1.23 3.3-1.23.66 1.65.24 2.88.12 3.18.765.84 1.23 1.905 1.23 3.225 0 4.605-2.805 5.625-5.475 5.925.435.375.81 1.095.81 2.22 0 1.605-.015 2.895-.015 3.3 0 .315.225.69.825.57A12.02 12.02 0 0 0 24 12c0-6.63-5.37-12-12-12z"/></svg>
            View Source on GitHub
          </a>
        </footer>
      </div>

      <style jsx global>{`
        *,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
        html{scroll-behavior:smooth}
        body{font-family:'Inter',-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;background:#060612;color:#d4d4f0;min-height:100vh}
        .page{position:relative;min-height:100vh;overflow-x:hidden}
        .orb{position:fixed;border-radius:50%;filter:blur(100px);opacity:.08;pointer-events:none;z-index:0;animation:float 25s ease-in-out infinite}
        .orb1{width:700px;height:700px;background:#5b5bd6;top:-300px;left:-200px}
        .orb2{width:500px;height:500px;background:#c026d3;bottom:-200px;right:-100px;animation-delay:-10s}
        .orb3{width:400px;height:400px;background:#0ea5e9;top:50%;left:45%;animation-delay:-18s}
        @keyframes float{0%,100%{transform:translate(0,0)}33%{transform:translate(20px,-20px)}66%{transform:translate(-15px,15px)}}
        .hdr{position:sticky;top:0;z-index:100;background:rgba(6,6,18,.9);backdrop-filter:blur(20px);border-bottom:1px solid rgba(255,255,255,.05)}
        .hdr-inner{max-width:1100px;margin:0 auto;padding:14px 24px;display:flex;align-items:center;justify-content:space-between;gap:12px;flex-wrap:wrap}
        .brand{display:flex;align-items:center;gap:11px}
        .brand-icon{font-size:30px}
        .brand-name{font-size:1.15em;font-weight:700;color:#fff;letter-spacing:-.3px}
        .brand-sub{font-size:.72em;color:rgba(255,255,255,.3);margin-top:1px}
        .hdr-right{display:flex;align-items:center;gap:10px}
        .gh-btn{display:flex;align-items:center;gap:6px;padding:7px 14px;background:rgba(255,255,255,.06);border:1px solid rgba(255,255,255,.1);border-radius:8px;color:rgba(255,255,255,.7);font-size:.8em;font-weight:500;text-decoration:none;transition:all .2s}
        .gh-btn:hover{background:rgba(255,255,255,.1);color:#fff}
        .status-pill{display:flex;align-items:center;gap:6px;padding:6px 13px;border-radius:20px;font-size:.78em;font-weight:600;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.08)}
        .status-pill.online{color:#4ade80;border-color:rgba(74,222,128,.25);background:rgba(74,222,128,.06)}
        .status-pill.offline,.status-pill.checking{color:#94a3b8}
        .sdot{width:6px;height:6px;border-radius:50%;background:currentColor;animation:pulse 2s infinite}
        @keyframes pulse{0%,100%{opacity:1}50%{opacity:.35}}
        .main{position:relative;z-index:1;max-width:1100px;margin:0 auto;padding:64px 24px 48px}
        .hero{text-align:center;margin-bottom:56px}
        .hero-eyebrow{font-size:.78em;font-weight:600;color:rgba(255,255,255,.35);letter-spacing:1.5px;text-transform:uppercase;margin-bottom:18px}
        .hero-h1{font-size:clamp(2em,5vw,3.4em);font-weight:800;line-height:1.15;color:#fff;margin-bottom:16px;letter-spacing:-.5px}
        .hero-grad{background:linear-gradient(135deg,#818cf8 0%,#a78bfa 40%,#e879f9 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text}
        .hero-p{max-width:600px;margin:0 auto 36px;color:rgba(255,255,255,.4);font-size:1.05em;line-height:1.75}
        .hero-metrics{display:flex;justify-content:center;gap:12px;flex-wrap:wrap}
        .metric-box{display:flex;flex-direction:column;align-items:center;gap:4px;padding:14px 20px;background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.07);border-radius:12px;min-width:100px}
        .metric-icon{font-size:20px}
        .metric-val{font-size:1.3em;font-weight:800;color:#fff}
        .metric-label{font-size:.7em;color:rgba(255,255,255,.35);text-transform:uppercase;letter-spacing:.5px}
        .tab-bar{display:flex;gap:4px;margin-bottom:28px;background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.07);border-radius:14px;padding:5px;width:fit-content}
        .tab-btn{display:flex;align-items:center;gap:8px;padding:10px 20px;border-radius:10px;font-size:.88em;font-weight:600;border:none;cursor:pointer;background:transparent;color:rgba(255,255,255,.4);transition:all .2s;white-space:nowrap;width:auto;transform:none}
        .tab-btn:hover{color:rgba(255,255,255,.8);background:rgba(255,255,255,.05);box-shadow:none;transform:none}
        .tab-active{background:rgba(129,140,248,.15)!important;color:#a5b4fc!important;border:1px solid rgba(129,140,248,.2)!important}
        .tab-icon{font-size:1em}
        .tab-badge{padding:2px 7px;border-radius:4px;font-size:.65em;font-weight:700;letter-spacing:.5px}
        .tab-badge.free{background:rgba(74,222,128,.15);color:#4ade80;border:1px solid rgba(74,222,128,.2)}
        .tab-badge.paid{background:rgba(251,191,36,.12);color:#fbbf24;border:1px solid rgba(251,191,36,.2)}
        .content-section{background:rgba(12,12,28,.7);backdrop-filter:blur(20px);border:1px solid rgba(255,255,255,.07);border-radius:20px;padding:32px;margin-bottom:24px}
        .section-hdr{display:flex;align-items:flex-start;justify-content:space-between;margin-bottom:24px;padding-bottom:20px;border-bottom:1px solid rgba(255,255,255,.06);gap:16px}
        .section-title{font-size:1.25em;font-weight:700;color:#fff;margin-bottom:4px}
        .section-sub{font-size:.82em;color:rgba(255,255,255,.35);line-height:1.5}
        .trial-badge{padding:6px 14px;border-radius:8px;font-size:.78em;font-weight:700;background:rgba(74,222,128,.1);color:#4ade80;border:1px solid rgba(74,222,128,.2);white-space:nowrap;height:fit-content}
        .premium-badge{padding:6px 14px;border-radius:8px;font-size:.78em;font-weight:700;background:rgba(251,191,36,.1);color:#fbbf24;border:1px solid rgba(251,191,36,.2);white-space:nowrap;height:fit-content}
        .loading-models{display:flex;flex-direction:column;align-items:center;justify-content:center;height:300px;gap:14px;color:rgba(255,255,255,.3);text-align:center}
        .loading-sub{font-size:.78em;color:rgba(255,255,255,.2)}
        .loader-ring{width:44px;height:44px;border:3px solid rgba(129,140,248,.15);border-top-color:#818cf8;border-radius:50%;animation:spin 1s linear infinite}
        @keyframes spin{to{transform:rotate(360deg)}}
        .progress-dots{display:flex;gap:6px}
        .progress-dots span{width:6px;height:6px;border-radius:50%;background:#818cf8;animation:blink 1.2s infinite}
        .progress-dots span:nth-child(2){animation-delay:.2s}
        .progress-dots span:nth-child(3){animation-delay:.4s}
        @keyframes blink{0%,80%,100%{opacity:.2}40%{opacity:1}}
        .cam-wrap{display:flex;flex-direction:column;gap:14px}
        .cam-vp{position:relative;width:100%;aspect-ratio:4/3;background:#000;border-radius:14px;overflow:hidden;border:1px solid rgba(255,255,255,.08);max-height:480px}
        .cam-vid{width:100%;height:100%;object-fit:cover;transform:scaleX(-1)}
        .cam-cvs{position:absolute;top:0;left:0;width:100%;height:100%;transform:scaleX(-1)}
        .cam-overlay{position:absolute;inset:0;display:flex;flex-direction:column;align-items:center;justify-content:center;background:rgba(6,6,18,.88);gap:14px;text-align:center;padding:24px}
        .cam-icon-wrap{width:72px;height:72px;border-radius:50%;background:rgba(129,140,248,.12);border:1px solid rgba(129,140,248,.2);display:flex;align-items:center;justify-content:center}
        .cam-big-icon{font-size:36px}
        .cam-title{font-size:1.2em;font-weight:700;color:#fff}
        .cam-desc{font-size:.85em;color:rgba(255,255,255,.38);max-width:340px;line-height:1.55}
        .primary-btn{display:flex;align-items:center;gap:8px;padding:12px 26px;background:linear-gradient(135deg,#5b5bd6,#8b5cf6);color:#fff;border:none;border-radius:10px;font-size:.95em;font-weight:600;cursor:pointer;transition:all .2s;width:auto;transform:none}
        .primary-btn:hover{transform:translateY(-2px);box-shadow:0 8px 24px rgba(91,91,214,.35)}
        .btn-arrow{font-size:1.1em}
        .cam-hud{position:absolute;top:10px;left:10px;right:10px;display:flex;gap:7px;flex-wrap:wrap;align-items:center}
        .hud-pill{padding:4px 11px;background:rgba(0,0,0,.65);backdrop-filter:blur(8px);border-radius:20px;font-size:.75em;font-weight:600;color:rgba(255,255,255,.8);border:1px solid rgba(255,255,255,.08)}
        .hud-emo{padding:4px 12px;background:rgba(0,0,0,.7);backdrop-filter:blur(8px);border-radius:20px;font-size:.78em;font-weight:700;border:1px solid;margin-left:auto}
        .cam-footer{}
        .live-bar-wrap{display:flex;align-items:center;gap:12px;background:rgba(12,12,28,.8);border:1px solid rgba(255,255,255,.07);border-radius:14px;padding:14px 18px}
        .live-emo-icon{font-size:32px;flex-shrink:0}
        .live-emo-info{flex:1}
        .live-emo-name{font-size:1em;font-weight:700;color:#fff;margin-bottom:6px}
        .live-emo-bar{height:5px;background:rgba(255,255,255,.08);border-radius:10px;overflow:hidden}
        .live-emo-fill{height:100%;border-radius:10px;transition:width .4s ease}
        .live-emo-pct{font-size:.9em;font-weight:700;color:#fff;min-width:40px;text-align:right}
        .stop-btn{padding:8px 16px;background:rgba(239,68,68,.1);color:#f87171;border:1px solid rgba(239,68,68,.2);border-radius:8px;font-size:.82em;font-weight:600;cursor:pointer;transition:all .2s;white-space:nowrap;transform:none;width:auto;box-shadow:none}
        .stop-btn:hover{background:rgba(239,68,68,.2);transform:none;box-shadow:none}
        .locked-wrap{position:relative;border-radius:14px;overflow:hidden}
        .locked-preview{padding:20px;filter:blur(3px);pointer-events:none;user-select:none;opacity:.4}
        .lp-row{display:grid;grid-template-columns:1fr 1fr;gap:16px}
        .lp-panel{background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.07);border-radius:10px;padding:16px;display:flex;flex-direction:column;gap:8px}
        .lp-label{font-size:.72em;color:rgba(255,255,255,.4);text-transform:uppercase;letter-spacing:.5px;font-weight:600}
        .lp-select{background:rgba(255,255,255,.05);border:1px solid rgba(255,255,255,.1);border-radius:8px;padding:10px 12px;font-size:.85em;color:#fff;display:flex;justify-content:space-between}
        .lp-textarea{background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.08);border-radius:8px;padding:10px;font-size:.72em;color:rgba(255,255,255,.5);font-family:monospace;height:80px;overflow:hidden}
        .lp-btn{background:linear-gradient(135deg,#5b5bd6,#8b5cf6);color:#fff;border-radius:8px;padding:11px;text-align:center;font-size:.85em;font-weight:600;margin-top:auto}
        .lp-result{display:flex;flex-direction:column;gap:8px;align-items:center}
        .lp-emo{font-size:48px}
        .lp-emo-name{font-size:1.2em;font-weight:700;color:#FFD93D}
        .lp-conf-track{width:100%;height:6px;background:rgba(255,255,255,.1);border-radius:10px;overflow:hidden}
        .lp-conf-fill{height:100%;width:94%;background:#FFD93D;border-radius:10px}
        .lp-json{font-size:.72em;color:#a8e6cf;font-family:monospace;background:rgba(0,0,0,.3);padding:10px;border-radius:8px;white-space:pre;width:100%}
        .lock-overlay{position:absolute;inset:0;display:flex;align-items:flex-start;justify-content:center;overflow-y:auto;background:rgba(6,6,18,.82);backdrop-filter:blur(6px);padding:24px}
        .lock-card{background:rgba(15,15,35,.95);border:1px solid rgba(255,255,255,.1);border-radius:18px;padding:32px;max-width:480px;text-align:center}
        .lock-icon{font-size:44px;margin-bottom:16px}
        .lock-title{font-size:1.3em;font-weight:700;color:#fff;margin-bottom:10px}
        .lock-desc{font-size:.88em;color:rgba(255,255,255,.45);line-height:1.6;margin-bottom:22px}
        .lock-features{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:24px;text-align:left}
        .lock-feat{font-size:.82em;color:#4ade80;font-weight:500}
        .lock-endpoints{display:flex;flex-direction:column;gap:8px;background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.07);border-radius:10px;padding:14px}
        .ep-row{display:flex;align-items:center;gap:10px;font-size:.8em}
        .ep-method{padding:2px 8px;border-radius:4px;font-size:.85em;font-weight:700;font-family:monospace;flex-shrink:0}
        .ep-method.get{background:rgba(74,222,128,.15);color:#4ade80}
        .ep-method.post{background:rgba(129,140,248,.15);color:#818cf8}
        .ep-row code{color:rgba(255,255,255,.7);font-family:monospace;flex:1}
        .ep-desc{color:rgba(255,255,255,.3);font-size:.9em}
        .sub-section-title{font-size:1em;font-weight:700;color:rgba(255,255,255,.6);text-transform:uppercase;letter-spacing:1px;font-size:.8em;margin:32px 0 16px}
        .sys-diagram{display:flex;align-items:center;justify-content:center;gap:0;flex-wrap:wrap;margin-bottom:8px;background:rgba(255,255,255,.02);border:1px solid rgba(255,255,255,.06);border-radius:14px;padding:24px}
        .sys-node{display:flex;flex-direction:column;align-items:center;gap:6px;padding:18px 24px;background:rgba(255,255,255,.04);border:1px solid rgba(255,255,255,.08);border-radius:12px;min-width:130px;text-align:center}
        .sys-node-icon{font-size:28px}
        .sys-node-label{font-size:.9em;font-weight:700;color:#fff}
        .sys-node-sub{font-size:.72em;color:rgba(255,255,255,.35)}
        .sys-arrow{display:flex;flex-direction:column;align-items:center;gap:4px;padding:0 12px}
        .sys-arrow-line{width:50px;height:2px;background:linear-gradient(90deg,rgba(129,140,248,.3),rgba(129,140,248,.6));position:relative}
        .sys-arrow-line::after{content:'›';position:absolute;right:-6px;top:-9px;color:#818cf8;font-size:18px}
        .sys-arrow-label{font-size:.65em;color:rgba(255,255,255,.3);white-space:nowrap}
        .patterns-grid{display:grid;grid-template-columns:1fr 1fr;gap:12px}
        @media(max-width:700px){.patterns-grid{grid-template-columns:1fr}}
        .pattern-card{background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.07);border-radius:12px;padding:18px;cursor:pointer;transition:all .2s}
        .pattern-card:hover,.pattern-open{border-color:rgba(129,140,248,.35);background:rgba(129,140,248,.06)}
        .pc-header{display:flex;align-items:center;justify-content:space-between;margin-bottom:8px;gap:10px}
        .pc-left{display:flex;align-items:center;gap:10px}
        .pc-icon{font-size:22px;flex-shrink:0}
        .pc-title{font-size:.95em;font-weight:700;color:#fff}
        .pc-tag{font-size:.68em;color:rgba(255,255,255,.35);text-transform:uppercase;letter-spacing:.5px;margin-top:2px}
        .pc-toggle{font-size:1.2em;color:rgba(255,255,255,.4);font-weight:300;width:20px;text-align:center;flex-shrink:0}
        .pc-desc{font-size:.82em;color:rgba(255,255,255,.4);line-height:1.55}
        .pc-code{margin-top:14px;background:rgba(0,0,0,.4);border:1px solid rgba(255,255,255,.08);border-radius:8px;padding:14px;font-size:.74em;color:#a8e6cf;font-family:'Monaco','Courier New',monospace;overflow-x:auto;white-space:pre;line-height:1.6}
        .pipeline{display:flex;align-items:flex-start;justify-content:center;flex-wrap:wrap;gap:0;background:rgba(255,255,255,.02);border:1px solid rgba(255,255,255,.06);border-radius:14px;padding:24px}
        .pipe-step{display:flex;flex-direction:column;align-items:center;gap:6px;text-align:center;position:relative}
        .pipe-node{width:52px;height:52px;border-radius:50%;background:rgba(129,140,248,.12);border:1px solid rgba(129,140,248,.25);display:flex;align-items:center;justify-content:center}
        .pipe-icon{font-size:22px}
        .pipe-connector{width:50px;height:2px;background:linear-gradient(90deg,rgba(129,140,248,.3),rgba(129,140,248,.5));margin-top:24px;flex-shrink:0}
        .pipe-label{font-size:.82em;font-weight:700;color:#fff;margin-top:4px}
        .pipe-sub{font-size:.68em;color:rgba(255,255,255,.3);max-width:90px}
        .file-tree{background:rgba(0,0,0,.4);border:1px solid rgba(255,255,255,.08);border-radius:12px;padding:20px;overflow-x:auto}
        .tree-code{font-size:.8em;color:#d4d4f0;font-family:'Monaco','Courier New',monospace;line-height:1.7;white-space:pre}
        .tech-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:14px}
        .tech-card{background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.07);border-radius:12px;padding:18px}
        .tech-cat{font-size:.72em;font-weight:700;color:rgba(255,255,255,.4);text-transform:uppercase;letter-spacing:.8px;margin-bottom:12px}
        .tech-items{display:flex;flex-wrap:wrap;gap:6px}
        .tech-pill{padding:4px 11px;background:rgba(129,140,248,.1);border:1px solid rgba(129,140,248,.2);border-radius:6px;font-size:.78em;color:#a5b4fc;font-weight:500}
        .footer{position:relative;z-index:1;text-align:center;padding:28px 24px;border-top:1px solid rgba(255,255,255,.05);color:rgba(255,255,255,.25);font-size:.85em;display:flex;flex-direction:column;gap:10px;align-items:center}
        .footer-link{display:flex;align-items:center;gap:6px;color:#818cf8;text-decoration:none;font-weight:500;font-size:.9em;transition:color .2s}
        .footer-link:hover{color:#a5b4fc}
        @media(max-width:768px){.sys-diagram{flex-direction:column}.sys-arrow{flex-direction:row;transform:rotate(90deg)}.lp-row{grid-template-columns:1fr}}
      `}</style>
    </>
  )
}
