const API_URL = window.location.origin;
let token = null;
let myColor = null;
let currentTurn = null;
const BOARD_SIZE = 9;
let pollInterval = null;

// HTML DOM Elementleri
const loginContainer = document.getElementById('login-container');
const gameContainer = document.getElementById('game-container');
const usernameInput = document.getElementById('username');
const passwordInput = document.getElementById('password');
const loginBtn = document.getElementById('login-btn');
const resetServerBtn = document.getElementById('reset-server-btn');
const loginError = document.getElementById('login-error');
const goBoard = document.getElementById('go-board');
const turnIndicator = document.getElementById('turn-indicator');
const statusBadge = document.getElementById('status-badge');
const gameMessage = document.getElementById('game-message');
const passBtn = document.getElementById('pass-btn');

// Tahtayı Başlat (Çiz)
// Bu fonksiyon, HTML'deki boş tabloya 9x9 (81 adet) hücre ekleyerek Go tahtasını çizer
function initBoard() {
    goBoard.innerHTML = ''; // Önce tahtayı temizle
    // CSS Grid (Izgara) kullanarak 9 sütun ve 9 satır oluştur (1fr = eşit oran)
    goBoard.style.gridTemplateColumns = `repeat(${BOARD_SIZE}, 1fr)`;
    goBoard.style.gridTemplateRows = `repeat(${BOARD_SIZE}, 1fr)`;

    for (let r = 0; r < BOARD_SIZE; r++) {
        for (let c = 0; c < BOARD_SIZE; c++) {
            const cell = document.createElement('div');
            cell.className = 'cell';
            
            // Kenar çizgilerini doğru çizebilmek için CSS sınıfları ekle
            if (r === 0) cell.classList.add('top');
            if (r === BOARD_SIZE - 1) cell.classList.add('bottom');
            if (c === 0) cell.classList.add('left');
            if (c === BOARD_SIZE - 1) cell.classList.add('right');

            const stone = document.createElement('div');
            stone.className = 'stone empty';
            stone.id = `stone-${r}-${c}`;
            
            cell.appendChild(stone);
            
            // Her hücreye tıklama (click) olay dinleyicisi ekle
            cell.addEventListener('click', () => {
                // Eğer sıra benim rengimde değilse hamle yapmayı engelle
                if (currentTurn !== myColor) {
                    showMessage("Sıra sizde değil!", true);
                    return;
                }
                // Tıklanan satır ve sütun bilgisini alıp hamleyi sunucuya gönder
                playMove(r, c);
            });

            goBoard.appendChild(cell);
        }
    }
}

function showMessage(msg, isError = false) {
    gameMessage.textContent = msg;
    gameMessage.style.color = isError ? '#ef4444' : '#facc15';
    setTimeout(() => { gameMessage.textContent = ''; }, 3000);
}

// API (Sunucu) İstekleri
// Kullanıcı adı ve şifreyle C++ sunucusuna giriş yapmayı sağlayan Asenkron (async) fonksiyon
async function login() {
    const u = usernameInput.value.trim(); // Boşlukları temizle
    const p = passwordInput.value.trim();
    if (!u || !p) return; // Boş bırakılmışsa işlem yapma

    try {
        // C++ sunucumuzdaki /login rotasına fetch komutuyla GET isteği yolla
        // 't' parametresi, tarayıcının önceki cevabı önbellekten (cache) getirmesini engeller (Date.now)
        const res = await fetch(`${API_URL}/login?u=${u}&p=${p}&t=${Date.now()}`);
        const data = await res.json();
        
        if (data.status === 'ok') {
            token = data.token;
            myColor = data.color;
            
            loginContainer.classList.remove('active');
            gameContainer.classList.add('active');
            
            document.getElementById('p1-info').innerHTML = `
                <div class="stone-icon ${myColor.toLowerCase()}"></div>
                <span>${myColor === 'Black' ? 'Siyah' : 'Beyaz'} (Siz)</span>
            `;
            const oppColor = myColor === 'Black' ? 'White' : 'Black';
            document.getElementById('p2-info').innerHTML = `
                <span>${oppColor === 'Black' ? 'Siyah' : 'Beyaz'} (Rakip)</span>
                <div class="stone-icon ${oppColor.toLowerCase()}"></div>
            `;
            
            initBoard(); // Giriş başarılıysa tahtayı ekrana çiz
            // POLLING: Saniyede 1 kere (1000ms) sunucuya "Yeni bir hamle var mı?" diye sor
            pollInterval = setInterval(fetchStatus, 1000);
            fetchStatus(); // Beklemeden ilk durumu hemen çek
        } else {
            loginError.textContent = data.msg;
        }
    } catch (err) {
        loginError.textContent = "Sunucuya bağlanılamadı. Sunucuyu başlattınız mı?";
    }
}

// Oyuncunun ekranını karşı tarafın hamleleriyle eşitleyen Sürekli Yoklama (Polling) fonksiyonu
async function fetchStatus() {
    if (!token) return; // Jeton (Token) yoksa sorgu yapma
    try {
        // Sunucunun /status rotasına sor ve C++'ın oluşturduğu JSON matrisini al
        const res = await fetch(`${API_URL}/status?token=${token}&t=${Date.now()}`);
        const data = await res.json(); // Gelen metni JavaScript nesnesine (Object) çevir
        
        updateBoard(data.board);
        
        currentTurn = data.turn;
        turnIndicator.textContent = `Sıra: ${currentTurn === 'Black' ? 'Siyah' : 'Beyaz'}`;
        
        if (data.playersReady) {
            statusBadge.textContent = "Bağlandı";
            statusBadge.className = "status-badge ready";
        } else {
            statusBadge.textContent = "Rakip Bekleniyor...";
            statusBadge.className = "status-badge";
        }

        if (data.gameOver) {
            clearInterval(pollInterval);
            turnIndicator.textContent = "Oyun Bitti!";
            showMessage("İki oyuncu da pas geçti. Oyun sona erdi.");
        }
    } catch (err) {
        console.error("Status fetch error", err);
    }
}

// C++ sunucusuna tıklanan koordinatları gönderip hamle yapan fonksiyon
async function playMove(r, c) {
    try {
        // Hangi satıra (r) ve sütuna (c) tıklandığını C++'ın /move adresine yolla
        const res = await fetch(`${API_URL}/move?token=${token}&r=${r}&c=${c}&t=${Date.now()}`);
        const data = await res.json();
        if (data.status === 'error') {
            showMessage(data.msg, true);
        } else {
            fetchStatus();
        }
    } catch (err) {
        showMessage("Hamle gönderilemedi.", true);
    }
}

async function passTurn() {
    if (currentTurn !== myColor) {
        showMessage("Sıra sizde değil!", true);
        return;
    }
    try {
        const res = await fetch(`${API_URL}/pass?token=${token}&t=${Date.now()}`);
        const data = await res.json();
        if (data.status === 'ok') {
            fetchStatus();
        } else {
            showMessage(data.msg, true);
        }
    } catch (err) {
        showMessage("Pas geçilemedi.", true);
    }
}

// C++'tan gelen 9x9'luk matrise (boardData) bakarak HTML ekranındaki taşların renklerini değiştirir
function updateBoard(boardData) {
    for (let r = 0; r < BOARD_SIZE; r++) {
        for (let c = 0; c < BOARD_SIZE; c++) {
            // HTML içindeki o spesifik hücrenin ID'sini bul
            const stone = document.getElementById(`stone-${r}-${c}`);
            const state = boardData[r][c]; // C++'ın yolladığı o hücredeki renk
            
            if (state === 'Black') {
                stone.className = 'stone black show';
            } else if (state === 'White') {
                stone.className = 'stone white show';
            } else {
                stone.className = 'stone empty';
            }
        }
    }
}

// Kullanıcı Olay Dinleyicileri (Tıklama vb.)
loginBtn.addEventListener('click', login);
resetServerBtn.addEventListener('click', async () => {
    try {
        await fetch(`${API_URL}/reset`);
        loginError.style.color = '#10b981'; // Yeşil renk
        loginError.textContent = "Oda başarıyla sıfırlandı! Artık girebilirsiniz.";
        setTimeout(() => { loginError.style.color = '#ef4444'; loginError.textContent = ''; }, 4000);
    } catch (err) {
        loginError.textContent = "Sıfırlama başarısız!";
    }
});
passwordInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') login();
});
passBtn.addEventListener('click', passTurn);
