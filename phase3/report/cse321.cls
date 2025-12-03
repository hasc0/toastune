\LoadClass[11pt]{article}

\RequirePackage{geometry}
\geometry{
    letterpaper,
    top=0.9in,
    bottom=0.9in,
    left=0.7in,
    right=0.7in,
    headheight=14pt,
    headsep=14pt,
}

\RequirePackage{fouriernc}
\RequirePackage[T1]{fontenc}
\RequirePackage[utf8]{inputenc}

\RequirePackage{amsmath}

\RequirePackage{hyperref}

\RequirePackage{graphicx}
\RequirePackage{caption}

\RequirePackage{listings}
\RequirePackage{xcolor}

\definecolor{bg}{rgb}{0.95,0.95,0.95}

\lstdefinestyle{code}{
    backgroundcolor=\color{bg},
    commentstyle=\color{gray},
    keywordstyle=\color{blue},
    numberstyle=\tiny\color{black},
    stringstyle=\color{green},
    basicstyle=\ttfamily\footnotesize,
    breakatwhitespace=false,
    breaklines=true,
    captionpos=t,
    keepspaces=true,
    numbers=left,
    numbersep=5pt,
    showspaces=false,
    showstringspaces=false,
    showtabs=false,
    tabsize=4
}

\lstset{style=code}

\RequirePackage{fancyhdr}
\pagestyle{fancy}
\fancyhf{}

\renewcommand{\headrulewidth}{0.5pt}
\renewcommand{\footrulewidth}{0.5pt}

\fancyhead[L]{CSE 321: Real-Time and Embedded Operating Systems}
\fancyhead[R]{Fall 2025}
\fancyfoot[C]{\thepage}

\renewcommand\maketitle{
    \begin{center}\LARGE\@title\end{center}
    \begin{center}\vspace{-7pt}\@author\end{center}
    \begin{center}\vspace{-9pt}\@date\end{center}
    \raisebox{10pt}{\rule{\textwidth}{0.5pt}}
    \vspace{-34pt}
}
