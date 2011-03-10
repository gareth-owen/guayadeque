// -------------------------------------------------------------------------------- //
//	Copyright (C) 2008-2011 J.Rios
//	anonbeat@gmail.com
//
//    This Program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2, or (at your option)
//    any later version.
//
//    This Program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; see the file LICENSE.  If not, write to
//    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//    http://www.gnu.org/copyleft/gpl.html
//
// -------------------------------------------------------------------------------- //
#include "CoListBox.h"

#include "Accelerators.h"
#include "Commands.h"
#include "Config.h"
#include "Images.h"
#include "OnlineLinks.h"
#include "MainApp.h"
#include "Utils.h"
#include "LibPanel.h"

// -------------------------------------------------------------------------------- //
guCoListBox::guCoListBox( wxWindow * parent, guLibPanel * libpanel, guDbLibrary * db, const wxString &label ) :
             guAccelListBox( parent, db, label )
{
    m_LibPanel = libpanel;

    CreateAcceleratorTable();

    ReloadItems();
}

// -------------------------------------------------------------------------------- //
guCoListBox::~guCoListBox()
{
}

// -------------------------------------------------------------------------------- //
void guCoListBox::CreateAcceleratorTable( void )
{
    wxAcceleratorTable AccelTable;
    wxArrayInt AliasAccelCmds;
    wxArrayInt RealAccelCmds;

    AliasAccelCmds.Add( ID_PLAYER_PLAYLIST_SAVE );
    AliasAccelCmds.Add( ID_PLAYER_PLAYLIST_EDITTRACKS );
    AliasAccelCmds.Add( ID_SONG_PLAY );
    AliasAccelCmds.Add( ID_SONG_ENQUEUE );
    AliasAccelCmds.Add( ID_SONG_ENQUEUE_ASNEXT );
    AliasAccelCmds.Add( ID_PLAYER_PLAYLIST_SEARCH );

    RealAccelCmds.Add( ID_COMPOSER_SAVETOPLAYLIST );
    RealAccelCmds.Add( ID_COMPOSER_EDITTRACKS );
    RealAccelCmds.Add( ID_COMPOSER_PLAY );
    RealAccelCmds.Add( ID_COMPOSER_ENQUEUE );
    RealAccelCmds.Add( ID_COMPOSER_ENQUEUE_ASNEXT );
    RealAccelCmds.Add( ID_LIBRARY_SEARCH );

    if( guAccelDoAcceleratorTable( AliasAccelCmds, RealAccelCmds, AccelTable ) )
    {
        SetAcceleratorTable( AccelTable );
    }
}

// -------------------------------------------------------------------------------- //
void guCoListBox::GetItemsList( void )
{
    m_Db->GetComposers( m_Items );
}

// -------------------------------------------------------------------------------- //
int guCoListBox::GetSelectedSongs( guTrackArray * songs ) const
{
    int Count = m_Db->GetComposersSongs( GetSelectedItems(), songs );
    m_LibPanel->NormalizeTracks( songs );
    return Count;
}

// -------------------------------------------------------------------------------- //
void guCoListBox::CreateContextMenu( wxMenu * Menu ) const
{
    wxMenuItem * MenuItem;

    int SelCount = GetSelectedCount();
    int ContextMenuFlags = m_LibPanel->GetContextMenuFlags();

    MenuItem = new wxMenuItem( Menu, ID_COMPOSER_PLAY,
                            wxString( _( "Play" ) ) + guAccelGetCommandKeyCodeString( ID_SONG_PLAY ),
                            _( "Play current selected composer" ) );
    MenuItem->SetBitmap( guImage( guIMAGE_INDEX_player_tiny_light_play ) );
    Menu->Append( MenuItem );

    MenuItem = new wxMenuItem( Menu, ID_COMPOSER_ENQUEUE,
                            wxString( _( "Enqueue" ) ) + guAccelGetCommandKeyCodeString( ID_SONG_ENQUEUE ),
                            _( "Add current selected tracks to playlist" ) );
    MenuItem->SetBitmap( guImage( guIMAGE_INDEX_tiny_add ) );
    Menu->Append( MenuItem );

    MenuItem = new wxMenuItem( Menu, ID_COMPOSER_ENQUEUE_ASNEXT,
                            wxString( _( "Enqueue Next" ) ) + guAccelGetCommandKeyCodeString( ID_SONG_ENQUEUE_ASNEXT ),
                            _( "Add current selected tracks to playlist as Next Tracks" ) );
    MenuItem->SetBitmap( guImage( guIMAGE_INDEX_tiny_add ) );
    Menu->Append( MenuItem );

    if( SelCount )
    {
        if( ContextMenuFlags & guLIBRARY_CONTEXTMENU_EDIT_TRACKS )
        {
            Menu->AppendSeparator();

            MenuItem = new wxMenuItem( Menu, ID_COMPOSER_EDITTRACKS,
                                    wxString( _( "Edit songs" ) ) + guAccelGetCommandKeyCodeString( ID_PLAYER_PLAYLIST_EDITTRACKS ),
                                    _( "Edit the selected tracks" ) );
            MenuItem->SetBitmap( guImage( guIMAGE_INDEX_tiny_edit ) );
            Menu->Append( MenuItem );
        }

        Menu->AppendSeparator();

        MenuItem = new wxMenuItem( Menu, ID_COMPOSER_SAVETOPLAYLIST,
                                wxString( _( "Save to PlayList" ) ) +  guAccelGetCommandKeyCodeString( ID_PLAYER_PLAYLIST_SAVE ),
                                _( "Save the selected tracks to PlayList" ) );
        MenuItem->SetBitmap( guImage( guIMAGE_INDEX_tiny_doc_save ) );
        Menu->Append( MenuItem );

        if( ContextMenuFlags & guLIBRARY_CONTEXTMENU_COPY_TO )
        {
            Menu->AppendSeparator();
            m_LibPanel->CreateCopyToMenu( Menu, ID_COMPOSER_COPYTO );
        }
    }

    m_LibPanel->CreateContextMenu( Menu );
}

// -------------------------------------------------------------------------------- //
wxString guCoListBox::GetSearchText( int item ) const
{
    return GetItemName( item );
}

// -------------------------------------------------------------------------------- //
int guCoListBox::GetDragFiles( wxFileDataObject * files )
{
    guTrackArray Songs;
    int index;
    int count = GetSelectedSongs( &Songs );
    m_LibPanel->NormalizeTracks( &Songs, true );
    for( index = 0; index < count; index++ )
    {
       wxString FileName = guFileDnDEncode( Songs[ index ].m_FileName );
       //FileName.Replace( wxT( "#" ), wxT( "%23" ) );
       //FileName.Replace( wxT( "%" ), wxT( "%25" ) );
       //guLogMessage( wxT( "Adding song '%s'" ), Songs[ index ].m_FileName.c_str() );
       files->AddFile( FileName );
    }
    return count;
}

// -------------------------------------------------------------------------------- //
int guCoListBox::FindComposer( const wxString &composer )
{
    int Index;
    int Count = m_Items->Count();
    for( Index = 0; Index < Count; Index++ )
    {
        if( m_Items->Item( Index ).m_Name == composer )
        {
            return Index;
        }
    }
    return wxNOT_FOUND;
}

// -------------------------------------------------------------------------------- //

