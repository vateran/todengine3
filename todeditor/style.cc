#include "todeditor/style.h"
namespace tod::editor
{

const wxColour Style::backgroundColor(32, 35, 44);
const wxColour Style::editboxBackgroundColor(22, 23, 29);
const wxColour Style::normalFontColor(251, 251, 251);
const wxColour Style::disableFontColor(128, 128, 128);
const wxColour Style::captionBackgroundColor(54, 58, 73);
const wxColour Style::captionFontColor(251, 251, 251);
const wxColour Style::componentCaptionBackgroundColor(65, 70, 88);
const wxFont Style::normalFont(wxFontInfo(10).FaceName("Consolas"));
const wxFont Style::captionFont(wxFontInfo(10).FaceName("Consolas").Bold());

}
