const API_BASE_URL = "";

const state = {
  selectedKeyword: "",
  keywords: [],
  activeSuggestionIndex: -1,
  suggestionRequestId: 0
};

const elements = {
  searchIntent: document.querySelector("#searchIntent"),
  searchButton: document.querySelector("#searchButton"),
  clearButton: document.querySelector("#clearButton"),
  keywordList: document.querySelector("#keywordList"),
  resultList: document.querySelector("#resultList"),
  resultMeta: document.querySelector("#resultMeta"),
  selectedKeywordValue: document.querySelector("#selectedKeywordValue"),
  connectionStatus: document.querySelector("#connectionStatus"),
  suggestionList: document.querySelector("#suggestionList")
};

elements.searchButton.addEventListener("click", handleSearch);
elements.clearButton.addEventListener("click", clearAll);
elements.searchIntent.addEventListener("input", handleInputSuggestion);
elements.searchIntent.addEventListener("keydown", handleInputKeydown);
elements.searchIntent.addEventListener("focus", () => {
  if (state.keywords.length) {
    renderSuggestions();
  }
});
document.addEventListener("click", handleDocumentClick);

async function handleInputSuggestion() {
  const intent = elements.searchIntent.value.trim();
  if (!intent) {
    state.keywords = [];
    state.activeSuggestionIndex = -1;
    hideSuggestions();
    renderEmptyKeywords("");
    state.selectedKeyword = "";
    elements.selectedKeywordValue.textContent = "";
    elements.searchButton.disabled = true;
    elements.resultMeta.textContent = "";
    renderEmptyResults("");
    return;
  }

  const requestId = ++state.suggestionRequestId;
  syncConnectionStatus("loading");

  try {
    const response = await requestKeywords(intent);
    if (requestId !== state.suggestionRequestId) {
      return;
    }

    state.keywords = response.recommendations || [];
    state.activeSuggestionIndex = -1;
    state.selectedKeyword = "";
    renderKeywords();
    resetResults();
    renderSuggestions();
    syncConnectionStatus("api");
  } catch (error) {
    console.error(error);
    if (requestId !== state.suggestionRequestId) {
      return;
    }

    state.keywords = [];
    state.activeSuggestionIndex = -1;
    hideSuggestions();
    renderEmptyKeywords("");
    syncConnectionStatus("error");
  }
}

async function handleSearch() {
  if (!state.selectedKeyword) {
    elements.resultMeta.textContent = "";
    return;
  }

  toggleSearchLoading(true);

  try {
    const response = await requestResults(state.selectedKeyword);
    renderResults(response);
    syncConnectionStatus("api");
  } catch (error) {
    console.error(error);
    elements.resultMeta.textContent = "";
    renderEmptyResults("");
    syncConnectionStatus("error");
  } finally {
    toggleSearchLoading(false);
  }
}

function selectKeyword(keyword) {
  state.selectedKeyword = keyword;
  elements.searchIntent.value = keyword;
  elements.selectedKeywordValue.textContent = keyword;
  elements.searchButton.disabled = false;
  hideSuggestions();
  renderKeywords();
}

function handleInputKeydown(event) {
  if (event.key === "ArrowDown") {
    if (!state.keywords.length) {
      return;
    }
    event.preventDefault();
    state.activeSuggestionIndex = (state.activeSuggestionIndex + 1) % state.keywords.length;
    renderSuggestions();
    return;
  }

  if (event.key === "ArrowUp") {
    if (!state.keywords.length) {
      return;
    }
    event.preventDefault();
    state.activeSuggestionIndex =
      state.activeSuggestionIndex <= 0 ? state.keywords.length - 1 : state.activeSuggestionIndex - 1;
    renderSuggestions();
    return;
  }

  if (event.key === "Enter") {
    if (state.activeSuggestionIndex >= 0 && state.keywords[state.activeSuggestionIndex]) {
      event.preventDefault();
      selectKeyword(state.keywords[state.activeSuggestionIndex]);
      handleSearch();
      return;
    }

    if (state.selectedKeyword) {
      event.preventDefault();
      handleSearch();
      return;
    }

    const typedKeyword = elements.searchIntent.value.trim();
    if (typedKeyword) {
      event.preventDefault();
      selectKeyword(typedKeyword);
      handleSearch();
    }
    return;
  }

  if (event.key === "Escape") {
    hideSuggestions();
  }
}

function renderKeywords() {
  if (!state.keywords.length) {
    renderEmptyKeywords("");
    return;
  }

  elements.keywordList.className = "keyword-list";
  elements.keywordList.innerHTML = "";

  state.keywords.forEach((keyword) => {
    const button = document.createElement("button");
    button.type = "button";
    button.className = `keyword-chip${keyword === state.selectedKeyword ? " is-active" : ""}`;
    button.textContent = keyword;
    button.addEventListener("click", () => selectKeyword(keyword));
    elements.keywordList.appendChild(button);
  });
}

function renderSuggestions() {
  if (!state.keywords.length) {
    hideSuggestions();
    return;
  }

  elements.suggestionList.hidden = false;
  elements.suggestionList.innerHTML = "";

  state.keywords.forEach((keyword, index) => {
    const button = document.createElement("button");
    button.type = "button";
    button.className = `suggestion-item${index === state.activeSuggestionIndex ? " is-active" : ""}`;
    button.innerHTML = `
      <span class="suggestion-icon">⌕</span>
      <span>${escapeHtml(keyword)}</span>
    `;
    button.addEventListener("click", () => selectKeyword(keyword));
    elements.suggestionList.appendChild(button);
  });
}

function renderResults(response) {
  const items = response.results || [];
  elements.resultMeta.textContent = `关键词「${response.keyword}」共返回 ${items.length} 条网页链接`;

  if (!items.length) {
    renderEmptyResults("");
    return;
  }

  elements.resultList.className = "result-list";
  elements.resultList.innerHTML = "";

  items.forEach((item) => {
    const card = document.createElement("article");
    card.className = "result-card";
    card.innerHTML = `
      <h3>${escapeHtml(item.title)}</h3>
      <p>${escapeHtml(item.description)}</p>
      <a href="${escapeAttribute(item.url)}" target="_blank" rel="noreferrer">${escapeHtml(item.url)}</a>
    `;
    elements.resultList.appendChild(card);
  });
}

function renderEmptyKeywords(message) {
  elements.keywordList.className = "keyword-list empty-state";
  elements.keywordList.textContent = message;
}

function renderEmptyResults(message) {
  elements.resultList.className = "result-list empty-state";
  elements.resultList.textContent = message;
}

function resetResults() {
  elements.searchButton.disabled = true;
  elements.selectedKeywordValue.textContent = "";
  elements.resultMeta.textContent = "";
  renderEmptyResults("");
}

function clearAll() {
  state.selectedKeyword = "";
  state.keywords = [];
  state.activeSuggestionIndex = -1;
  state.suggestionRequestId += 1;
  elements.searchIntent.value = "";
  hideSuggestions();
  renderEmptyKeywords("");
  resetResults();
  syncConnectionStatus("idle");
}

function toggleSearchLoading(isLoading) {
  elements.searchButton.disabled = isLoading || !state.selectedKeyword;
  elements.searchButton.textContent = isLoading ? "查询中..." : "查询链接";
}

function syncConnectionStatus(source) {
  if (source === "api") {
    elements.connectionStatus.textContent = "已连接后端";
    return;
  }

  if (source === "loading") {
    elements.connectionStatus.textContent = "请求推荐词中";
    return;
  }

  if (source === "error") {
    elements.connectionStatus.textContent = "后端未连接";
    return;
  }

  elements.connectionStatus.textContent = "等待输入";
}

async function requestKeywords(intent) {
  const response = await fetch(`${API_BASE_URL}/api/v1/keywords/recommendations`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json"
    },
    body: JSON.stringify({ intent, size: 8 })
  });

  if (!response.ok) {
    throw new Error(`Keyword request failed with status ${response.status}`);
  }

  const data = await response.json();
  return {
    recommendations: data.data?.recommendations || data.recommendations || []
  };
}

async function requestResults(keyword) {
  const response = await fetch(`${API_BASE_URL}/api/v1/search/results`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json"
    },
    body: JSON.stringify({ keyword, page: 1, pageSize: 10 })
  });

  if (!response.ok) {
    throw new Error(`Result request failed with status ${response.status}`);
  }

  const data = await response.json();
  return {
    keyword,
    results: data.data?.results || data.results || []
  };
}

function hideSuggestions() {
  elements.suggestionList.hidden = true;
  elements.suggestionList.innerHTML = "";
}

function handleDocumentClick(event) {
  if (
    event.target === elements.searchIntent ||
    elements.suggestionList.contains(event.target)
  ) {
    return;
  }

  hideSuggestions();
}

function escapeHtml(value) {
  return String(value)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#39;");
}

function escapeAttribute(value) {
  return escapeHtml(value);
}

syncConnectionStatus("idle");
renderEmptyKeywords("");
renderEmptyResults("");

// Initialize ParticleJS for cyberpunk feel
document.addEventListener("DOMContentLoaded", function() {
  if (window.particlesJS) {
    particlesJS("particles-js", {
      "particles": {
        "number": {
          "value": 150,
          "density": { "enable": true, "value_area": 800 }
        },
        "color": { "value": ["#00ffff", "#ff00ff", "#0077ff"] },
        "shape": {
          "type": "circle",
          "stroke": { "width": 0, "color": "#000000" },
          "polygon": { "nb_sides": 5 }
        },
        "opacity": {
          "value": 0.8,
          "random": true,
          "anim": { "enable": true, "speed": 1, "opacity_min": 0.1, "sync": false }
        },
        "size": {
          "value": 3,
          "random": true,
          "anim": { "enable": true, "speed": 3, "size_min": 0.1, "sync": false }
        },
        "line_linked": {
          "enable": true,
          "distance": 150,
          "color": "#00ffff",
          "opacity": 0.4,
          "width": 1.5
        },
        "move": {
          "enable": true,
          "speed": 3,
          "direction": "none",
          "random": false,
          "straight": false,
          "out_mode": "out",
          "bounce": false,
          "attract": { "enable": true, "rotateX": 600, "rotateY": 1200 }
        }
      },
      "interactivity": {
        "detect_on": "canvas",
        "events": {
          "onhover": { "enable": true, "mode": "grab" },
          "onclick": { "enable": true, "mode": "repulse" },
          "resize": true
        },
        "modes": {
          "grab": { "distance": 200, "line_linked": { "opacity": 0.8 } },
          "bubble": { "distance": 400, "size": 40, "duration": 2, "opacity": 8, "speed": 3 },
          "repulse": { "distance": 200, "duration": 0.4 },
          "push": { "particles_nb": 4 },
          "remove": { "particles_nb": 2 }
        }
      },
      "retina_detect": true
    });
  }

  // Add click sound effects or interactions if needed
  const buttons = document.querySelectorAll('.cyber-btn');
  buttons.forEach(btn => {
    btn.addEventListener('click', () => {
      btn.style.transform = 'scale(0.95)';
      setTimeout(() => btn.style.transform = 'scale(1)', 100);
    });
  });
});
